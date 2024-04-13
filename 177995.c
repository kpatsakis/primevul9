int phar_open_entry_fp(phar_entry_info *entry, char **error, int follow_links) /* {{{ */
{
	php_stream_filter *filter;
	phar_archive_data *phar = entry->phar;
	char *filtername;
	zend_off_t loc;
	php_stream *ufp;
	phar_entry_data dummy;

	if (follow_links && entry->link) {
		phar_entry_info *link_entry = phar_get_link_source(entry);
		if (link_entry && link_entry != entry) {
			return phar_open_entry_fp(link_entry, error, 1);
		}
	}

	if (entry->is_modified) {
		return SUCCESS;
	}

	if (entry->fp_type == PHAR_TMP) {
		if (!entry->fp) {
			entry->fp = php_stream_open_wrapper(entry->tmp, "rb", STREAM_MUST_SEEK|0, NULL);
		}
		return SUCCESS;
	}

	if (entry->fp_type != PHAR_FP) {
		/* either newly created or already modified */
		return SUCCESS;
	}

	if (!phar_get_pharfp(phar)) {
		if (FAILURE == phar_open_archive_fp(phar)) {
			spprintf(error, 4096, "phar error: Cannot open phar archive \"%s\" for reading", phar->fname);
			return FAILURE;
		}
	}

	if ((entry->old_flags && !(entry->old_flags & PHAR_ENT_COMPRESSION_MASK)) || !(entry->flags & PHAR_ENT_COMPRESSION_MASK)) {
		dummy.internal_file = entry;
		dummy.phar = phar;
		dummy.zero = entry->offset;
		dummy.fp = phar_get_pharfp(phar);
		if (FAILURE == phar_postprocess_file(&dummy, entry->crc32, error, 1)) {
			return FAILURE;
		}
		return SUCCESS;
	}

	if (!phar_get_entrypufp(entry)) {
		phar_set_entrypufp(entry, php_stream_fopen_tmpfile());
		if (!phar_get_entrypufp(entry)) {
			spprintf(error, 4096, "phar error: Cannot open temporary file for decompressing phar archive \"%s\" file \"%s\"", phar->fname, entry->filename);
			return FAILURE;
		}
	}

	dummy.internal_file = entry;
	dummy.phar = phar;
	dummy.zero = entry->offset;
	dummy.fp = phar_get_pharfp(phar);
	if (FAILURE == phar_postprocess_file(&dummy, entry->crc32, error, 1)) {
		return FAILURE;
	}

	ufp = phar_get_entrypufp(entry);

	if ((filtername = phar_decompress_filter(entry, 0)) != NULL) {
		filter = php_stream_filter_create(filtername, NULL, 0);
	} else {
		filter = NULL;
	}

	if (!filter) {
		spprintf(error, 4096, "phar error: unable to read phar \"%s\" (cannot create %s filter while decompressing file \"%s\")", phar->fname, phar_decompress_filter(entry, 1), entry->filename);
		return FAILURE;
	}

	/* now we can safely use proper decompression */
	/* save the new offset location within ufp */
	php_stream_seek(ufp, 0, SEEK_END);
	loc = php_stream_tell(ufp);
	php_stream_filter_append(&ufp->writefilters, filter);
	php_stream_seek(phar_get_entrypfp(entry), phar_get_fp_offset(entry), SEEK_SET);

	if (entry->uncompressed_filesize) {
		if (SUCCESS != php_stream_copy_to_stream_ex(phar_get_entrypfp(entry), ufp, entry->compressed_filesize, NULL)) {
			spprintf(error, 4096, "phar error: internal corruption of phar \"%s\" (actual filesize mismatch on file \"%s\")", phar->fname, entry->filename);
			php_stream_filter_remove(filter, 1);
			return FAILURE;
		}
	}

	php_stream_filter_flush(filter, 1);
	php_stream_flush(ufp);
	php_stream_filter_remove(filter, 1);

	if (php_stream_tell(ufp) - loc != (zend_off_t) entry->uncompressed_filesize) {
		spprintf(error, 4096, "phar error: internal corruption of phar \"%s\" (actual filesize mismatch on file \"%s\")", phar->fname, entry->filename);
		return FAILURE;
	}

	entry->old_flags = entry->flags;

	/* this is now the new location of the file contents within this fp */
	phar_set_fp_type(entry, PHAR_UFP, loc);
	dummy.zero = entry->offset;
	dummy.fp = ufp;
	if (FAILURE == phar_postprocess_file(&dummy, entry->crc32, error, 0)) {
		return FAILURE;
	}
	return SUCCESS;
}