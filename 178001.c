int phar_parse_zipfile(php_stream *fp, char *fname, int fname_len, char *alias, int alias_len, phar_archive_data** pphar, char **error) /* {{{ */
{
	phar_zip_dir_end locator;
	char buf[sizeof(locator) + 65536];
	zend_long size;
	php_uint16 i;
	phar_archive_data *mydata = NULL;
	phar_entry_info entry = {0};
	char *p = buf, *ext, *actual_alias = NULL;
	char *metadata = NULL;

	size = php_stream_tell(fp);

	if (size > sizeof(locator) + 65536) {
		/* seek to max comment length + end of central directory record */
		size = sizeof(locator) + 65536;
		if (FAILURE == php_stream_seek(fp, -size, SEEK_END)) {
			php_stream_close(fp);
			if (error) {
				spprintf(error, 4096, "phar error: unable to search for end of central directory in zip-based phar \"%s\"", fname);
			}
			return FAILURE;
		}
	} else {
		php_stream_seek(fp, 0, SEEK_SET);
	}

	if (!php_stream_read(fp, buf, size)) {
		php_stream_close(fp);
		if (error) {
			spprintf(error, 4096, "phar error: unable to read in data to search for end of central directory in zip-based phar \"%s\"", fname);
		}
		return FAILURE;
	}

	while ((p=(char *) memchr(p + 1, 'P', (size_t) (size - (p + 1 - buf)))) != NULL) {
		if ((p - buf) + sizeof(locator) <= size && !memcmp(p + 1, "K\5\6", 3)) {
			memcpy((void *)&locator, (void *) p, sizeof(locator));
			if (PHAR_GET_16(locator.centraldisk) != 0 || PHAR_GET_16(locator.disknumber) != 0) {
				/* split archives not handled */
				php_stream_close(fp);
				if (error) {
					spprintf(error, 4096, "phar error: split archives spanning multiple zips cannot be processed in zip-based phar \"%s\"", fname);
				}
				return FAILURE;
			}

			if (PHAR_GET_16(locator.counthere) != PHAR_GET_16(locator.count)) {
				if (error) {
					spprintf(error, 4096, "phar error: corrupt zip archive, conflicting file count in end of central directory record in zip-based phar \"%s\"", fname);
				}
				php_stream_close(fp);
				return FAILURE;
			}

			mydata = pecalloc(1, sizeof(phar_archive_data), PHAR_G(persist));
			mydata->is_persistent = PHAR_G(persist);

			/* read in archive comment, if any */
			if (PHAR_GET_16(locator.comment_len)) {

				metadata = p + sizeof(locator);

				if (PHAR_GET_16(locator.comment_len) != size - (metadata - buf)) {
					if (error) {
						spprintf(error, 4096, "phar error: corrupt zip archive, zip file comment truncated in zip-based phar \"%s\"", fname);
					}
					php_stream_close(fp);
					pefree(mydata, mydata->is_persistent);
					return FAILURE;
				}

				mydata->metadata_len = PHAR_GET_16(locator.comment_len);

				if (phar_parse_metadata(&metadata, &mydata->metadata, PHAR_GET_16(locator.comment_len)) == FAILURE) {
					mydata->metadata_len = 0;
					/* if not valid serialized data, it is a regular string */

					ZVAL_NEW_STR(&mydata->metadata, zend_string_init(metadata, PHAR_GET_16(locator.comment_len), mydata->is_persistent));
				}
			} else {
				ZVAL_UNDEF(&mydata->metadata);
			}

			goto foundit;
		}
	}

	php_stream_close(fp);

	if (error) {
		spprintf(error, 4096, "phar error: end of central directory not found in zip-based phar \"%s\"", fname);
	}

	return FAILURE;
foundit:
	mydata->fname = pestrndup(fname, fname_len, mydata->is_persistent);
#ifdef PHP_WIN32
	phar_unixify_path_separators(mydata->fname, fname_len);
#endif
	mydata->is_zip = 1;
	mydata->fname_len = fname_len;
	ext = strrchr(mydata->fname, '/');

	if (ext) {
		mydata->ext = memchr(ext, '.', (mydata->fname + fname_len) - ext);
		if (mydata->ext == ext) {
			mydata->ext = memchr(ext + 1, '.', (mydata->fname + fname_len) - ext - 1);
		}
		if (mydata->ext) {
			mydata->ext_len = (mydata->fname + fname_len) - mydata->ext;
		}
	}

	/* clean up on big-endian systems */
	/* seek to central directory */
	php_stream_seek(fp, PHAR_GET_32(locator.cdir_offset), SEEK_SET);
	/* read in central directory */
	zend_hash_init(&mydata->manifest, PHAR_GET_16(locator.count),
		zend_get_hash_value, destroy_phar_manifest_entry, (zend_bool)mydata->is_persistent);
	zend_hash_init(&mydata->mounted_dirs, 5,
		zend_get_hash_value, NULL, (zend_bool)mydata->is_persistent);
	zend_hash_init(&mydata->virtual_dirs, PHAR_GET_16(locator.count) * 2,
		zend_get_hash_value, NULL, (zend_bool)mydata->is_persistent);
	entry.phar = mydata;
	entry.is_zip = 1;
	entry.fp_type = PHAR_FP;
	entry.is_persistent = mydata->is_persistent;
#define PHAR_ZIP_FAIL_FREE(errmsg, save) \
			zend_hash_destroy(&mydata->manifest); \
			mydata->manifest.u.flags = 0; \
			zend_hash_destroy(&mydata->mounted_dirs); \
			mydata->mounted_dirs.u.flags = 0; \
			zend_hash_destroy(&mydata->virtual_dirs); \
			mydata->virtual_dirs.u.flags = 0; \
			php_stream_close(fp); \
			zval_dtor(&mydata->metadata); \
			if (mydata->signature) { \
				efree(mydata->signature); \
			} \
			if (error) { \
				spprintf(error, 4096, "phar error: %s in zip-based phar \"%s\"", errmsg, mydata->fname); \
			} \
			pefree(mydata->fname, mydata->is_persistent); \
			if (mydata->alias) { \
				pefree(mydata->alias, mydata->is_persistent); \
			} \
			pefree(mydata, mydata->is_persistent); \
			efree(save); \
			return FAILURE;
#define PHAR_ZIP_FAIL(errmsg) \
			zend_hash_destroy(&mydata->manifest); \
			mydata->manifest.u.flags = 0; \
			zend_hash_destroy(&mydata->mounted_dirs); \
			mydata->mounted_dirs.u.flags = 0; \
			zend_hash_destroy(&mydata->virtual_dirs); \
			mydata->virtual_dirs.u.flags = 0; \
			php_stream_close(fp); \
			zval_dtor(&mydata->metadata); \
			if (mydata->signature) { \
				efree(mydata->signature); \
			} \
			if (error) { \
				spprintf(error, 4096, "phar error: %s in zip-based phar \"%s\"", errmsg, mydata->fname); \
			} \
			pefree(mydata->fname, mydata->is_persistent); \
			if (mydata->alias) { \
				pefree(mydata->alias, mydata->is_persistent); \
			} \
			pefree(mydata, mydata->is_persistent); \
			return FAILURE;

	/* add each central directory item to the manifest */
	for (i = 0; i < PHAR_GET_16(locator.count); ++i) {
		phar_zip_central_dir_file zipentry;
		zend_off_t beforeus = php_stream_tell(fp);

		if (sizeof(zipentry) != php_stream_read(fp, (char *) &zipentry, sizeof(zipentry))) {
			PHAR_ZIP_FAIL("unable to read central directory entry, truncated");
		}

		/* clean up for bigendian systems */
		if (memcmp("PK\1\2", zipentry.signature, 4)) {
			/* corrupted entry */
			PHAR_ZIP_FAIL("corrupted central directory entry, no magic signature");
		}

		if (entry.is_persistent) {
			entry.manifest_pos = i;
		}

		entry.compressed_filesize = PHAR_GET_32(zipentry.compsize);
		entry.uncompressed_filesize = PHAR_GET_32(zipentry.uncompsize);
		entry.crc32 = PHAR_GET_32(zipentry.crc32);
		/* do not PHAR_GET_16 either on the next line */
		entry.timestamp = phar_zip_d2u_time(zipentry.timestamp, zipentry.datestamp);
		entry.flags = PHAR_ENT_PERM_DEF_FILE;
		entry.header_offset = PHAR_GET_32(zipentry.offset);
		entry.offset = entry.offset_abs = PHAR_GET_32(zipentry.offset) + sizeof(phar_zip_file_header) + PHAR_GET_16(zipentry.filename_len) +
			PHAR_GET_16(zipentry.extra_len);

		if (PHAR_GET_16(zipentry.flags) & PHAR_ZIP_FLAG_ENCRYPTED) {
			PHAR_ZIP_FAIL("Cannot process encrypted zip files");
		}

		if (!PHAR_GET_16(zipentry.filename_len)) {
			PHAR_ZIP_FAIL("Cannot process zips created from stdin (zero-length filename)");
		}

		entry.filename_len = PHAR_GET_16(zipentry.filename_len);
		entry.filename = (char *) pemalloc(entry.filename_len + 1, entry.is_persistent);

		if (entry.filename_len != php_stream_read(fp, entry.filename, entry.filename_len)) {
			pefree(entry.filename, entry.is_persistent);
			PHAR_ZIP_FAIL("unable to read in filename from central directory, truncated");
		}

		entry.filename[entry.filename_len] = '\0';

		if (entry.filename[entry.filename_len - 1] == '/') {
			entry.is_dir = 1;
			if(entry.filename_len > 1) {
				entry.filename_len--;
			}
			entry.flags |= PHAR_ENT_PERM_DEF_DIR;
		} else {
			entry.is_dir = 0;
		}

		if (entry.filename_len == sizeof(".phar/signature.bin")-1 && !strncmp(entry.filename, ".phar/signature.bin", sizeof(".phar/signature.bin")-1)) {
			size_t read;
			php_stream *sigfile;
			zend_off_t now;
			char *sig;

			now = php_stream_tell(fp);
			pefree(entry.filename, entry.is_persistent);
			sigfile = php_stream_fopen_tmpfile();
			if (!sigfile) {
				PHAR_ZIP_FAIL("couldn't open temporary file");
			}

			php_stream_seek(fp, 0, SEEK_SET);
			/* copy file contents + local headers and zip comment, if any, to be hashed for signature */
			php_stream_copy_to_stream_ex(fp, sigfile, entry.header_offset, NULL);
			/* seek to central directory */
			php_stream_seek(fp, PHAR_GET_32(locator.cdir_offset), SEEK_SET);
			/* copy central directory header */
			php_stream_copy_to_stream_ex(fp, sigfile, beforeus - PHAR_GET_32(locator.cdir_offset), NULL);
			if (metadata) {
				php_stream_write(sigfile, metadata, PHAR_GET_16(locator.comment_len));
			}
			php_stream_seek(fp, sizeof(phar_zip_file_header) + entry.header_offset + entry.filename_len + PHAR_GET_16(zipentry.extra_len), SEEK_SET);
			sig = (char *) emalloc(entry.uncompressed_filesize);
			read = php_stream_read(fp, sig, entry.uncompressed_filesize);
			if (read != entry.uncompressed_filesize || read <= 8) {
				php_stream_close(sigfile);
				efree(sig);
				PHAR_ZIP_FAIL("signature cannot be read");
			}
			mydata->sig_flags = PHAR_GET_32(sig);
			if (FAILURE == phar_verify_signature(sigfile, php_stream_tell(sigfile), mydata->sig_flags, sig + 8, entry.uncompressed_filesize - 8, fname, &mydata->signature, &mydata->sig_len, error)) {
				efree(sig);
				if (error) {
					char *save;
					php_stream_close(sigfile);
					spprintf(&save, 4096, "signature cannot be verified: %s", *error);
					efree(*error);
					PHAR_ZIP_FAIL_FREE(save, save);
				} else {
					php_stream_close(sigfile);
					PHAR_ZIP_FAIL("signature cannot be verified");
				}
			}
			php_stream_close(sigfile);
			efree(sig);
			/* signature checked out, let's ensure this is the last file in the phar */
			if (i != PHAR_GET_16(locator.count) - 1) {
				PHAR_ZIP_FAIL("entries exist after signature, invalid phar");
			}

			continue;
		}

		phar_add_virtual_dirs(mydata, entry.filename, entry.filename_len);

		if (PHAR_GET_16(zipentry.extra_len)) {
			zend_off_t loc = php_stream_tell(fp);
			if (FAILURE == phar_zip_process_extra(fp, &entry, PHAR_GET_16(zipentry.extra_len))) {
				pefree(entry.filename, entry.is_persistent);
				PHAR_ZIP_FAIL("Unable to process extra field header for file in central directory");
			}
			php_stream_seek(fp, loc + PHAR_GET_16(zipentry.extra_len), SEEK_SET);
		}

		switch (PHAR_GET_16(zipentry.compressed)) {
			case PHAR_ZIP_COMP_NONE :
				/* compression flag already set */
				break;
			case PHAR_ZIP_COMP_DEFLATE :
				entry.flags |= PHAR_ENT_COMPRESSED_GZ;
				if (!PHAR_G(has_zlib)) {
					pefree(entry.filename, entry.is_persistent);
					PHAR_ZIP_FAIL("zlib extension is required");
				}
				break;
			case PHAR_ZIP_COMP_BZIP2 :
				entry.flags |= PHAR_ENT_COMPRESSED_BZ2;
				if (!PHAR_G(has_bz2)) {
					pefree(entry.filename, entry.is_persistent);
					PHAR_ZIP_FAIL("bzip2 extension is required");
				}
				break;
			case 1 :
				pefree(entry.filename, entry.is_persistent);
				PHAR_ZIP_FAIL("unsupported compression method (Shrunk) used in this zip");
			case 2 :
			case 3 :
			case 4 :
			case 5 :
				pefree(entry.filename, entry.is_persistent);
				PHAR_ZIP_FAIL("unsupported compression method (Reduce) used in this zip");
			case 6 :
				pefree(entry.filename, entry.is_persistent);
				PHAR_ZIP_FAIL("unsupported compression method (Implode) used in this zip");
			case 7 :
				pefree(entry.filename, entry.is_persistent);
				PHAR_ZIP_FAIL("unsupported compression method (Tokenize) used in this zip");
			case 9 :
				pefree(entry.filename, entry.is_persistent);
				PHAR_ZIP_FAIL("unsupported compression method (Deflate64) used in this zip");
			case 10 :
				pefree(entry.filename, entry.is_persistent);
				PHAR_ZIP_FAIL("unsupported compression method (PKWare Implode/old IBM TERSE) used in this zip");
			case 14 :
				pefree(entry.filename, entry.is_persistent);
				PHAR_ZIP_FAIL("unsupported compression method (LZMA) used in this zip");
			case 18 :
				pefree(entry.filename, entry.is_persistent);
				PHAR_ZIP_FAIL("unsupported compression method (IBM TERSE) used in this zip");
			case 19 :
				pefree(entry.filename, entry.is_persistent);
				PHAR_ZIP_FAIL("unsupported compression method (IBM LZ77) used in this zip");
			case 97 :
				pefree(entry.filename, entry.is_persistent);
				PHAR_ZIP_FAIL("unsupported compression method (WavPack) used in this zip");
			case 98 :
				pefree(entry.filename, entry.is_persistent);
				PHAR_ZIP_FAIL("unsupported compression method (PPMd) used in this zip");
			default :
				pefree(entry.filename, entry.is_persistent);
				PHAR_ZIP_FAIL("unsupported compression method (unknown) used in this zip");
		}

		/* get file metadata */
		if (PHAR_GET_16(zipentry.comment_len)) {
			if (PHAR_GET_16(zipentry.comment_len) != php_stream_read(fp, buf, PHAR_GET_16(zipentry.comment_len))) {
				pefree(entry.filename, entry.is_persistent);
				PHAR_ZIP_FAIL("unable to read in file comment, truncated");
			}

			p = buf;
			entry.metadata_len = PHAR_GET_16(zipentry.comment_len);

			if (phar_parse_metadata(&p, &(entry.metadata), PHAR_GET_16(zipentry.comment_len)) == FAILURE) {
				entry.metadata_len = 0;
				/* if not valid serialized data, it is a regular string */

				ZVAL_NEW_STR(&entry.metadata, zend_string_init(buf, PHAR_GET_16(zipentry.comment_len), entry.is_persistent));
			}
		} else {
			ZVAL_UNDEF(&entry.metadata);
		}

		if (!actual_alias && entry.filename_len == sizeof(".phar/alias.txt")-1 && !strncmp(entry.filename, ".phar/alias.txt", sizeof(".phar/alias.txt")-1)) {
			php_stream_filter *filter;
			zend_off_t saveloc;
			/* verify local file header */
			phar_zip_file_header local;

			/* archive alias found */
			saveloc = php_stream_tell(fp);
			php_stream_seek(fp, PHAR_GET_32(zipentry.offset), SEEK_SET);

			if (sizeof(local) != php_stream_read(fp, (char *) &local, sizeof(local))) {
				pefree(entry.filename, entry.is_persistent);
				PHAR_ZIP_FAIL("phar error: internal corruption of zip-based phar (cannot read local file header for alias)");
			}

			/* verify local header */
			if (entry.filename_len != PHAR_GET_16(local.filename_len) || entry.crc32 != PHAR_GET_32(local.crc32) || entry.uncompressed_filesize != PHAR_GET_32(local.uncompsize) || entry.compressed_filesize != PHAR_GET_32(local.compsize)) {
				pefree(entry.filename, entry.is_persistent);
				PHAR_ZIP_FAIL("phar error: internal corruption of zip-based phar (local header of alias does not match central directory)");
			}

			/* construct actual offset to file start - local extra_len can be different from central extra_len */
			entry.offset = entry.offset_abs =
				sizeof(local) + entry.header_offset + PHAR_GET_16(local.filename_len) + PHAR_GET_16(local.extra_len);
			php_stream_seek(fp, entry.offset, SEEK_SET);
			/* these next lines should be for php < 5.2.6 after 5.3 filters are fixed */
			fp->writepos = 0;
			fp->readpos = 0;
			php_stream_seek(fp, entry.offset, SEEK_SET);
			fp->writepos = 0;
			fp->readpos = 0;
			/* the above lines should be for php < 5.2.6 after 5.3 filters are fixed */

			mydata->alias_len = entry.uncompressed_filesize;
			if (entry.flags & PHAR_ENT_COMPRESSED_GZ) {
				filter = php_stream_filter_create("zlib.inflate", NULL, php_stream_is_persistent(fp));

				if (!filter) {
					pefree(entry.filename, entry.is_persistent);
					PHAR_ZIP_FAIL("unable to decompress alias, zlib filter creation failed");
				}

				php_stream_filter_append(&fp->readfilters, filter);

				// TODO: refactor to avoid reallocation ???
//???			entry.uncompressed_filesize = php_stream_copy_to_mem(fp, &actual_alias, entry.uncompressed_filesize, 0)
				{
					zend_string *str = php_stream_copy_to_mem(fp, entry.uncompressed_filesize, 0);
					if (str) {
						entry.uncompressed_filesize = ZSTR_LEN(str);
						actual_alias = estrndup(ZSTR_VAL(str), ZSTR_LEN(str));
						zend_string_release(str);
					} else {
						actual_alias = NULL;
						entry.uncompressed_filesize = 0;
					}
				}

				if (!entry.uncompressed_filesize || !actual_alias) {
					pefree(entry.filename, entry.is_persistent);
					PHAR_ZIP_FAIL("unable to read in alias, truncated");
				}

				php_stream_filter_flush(filter, 1);
				php_stream_filter_remove(filter, 1);

			} else if (entry.flags & PHAR_ENT_COMPRESSED_BZ2) {
				filter = php_stream_filter_create("bzip2.decompress", NULL, php_stream_is_persistent(fp));

				if (!filter) {
					pefree(entry.filename, entry.is_persistent);
					PHAR_ZIP_FAIL("unable to read in alias, bzip2 filter creation failed");
				}

				php_stream_filter_append(&fp->readfilters, filter);

				// TODO: refactor to avoid reallocation ???
//???			entry.uncompressed_filesize = php_stream_copy_to_mem(fp, &actual_alias, entry.uncompressed_filesize, 0)
				{
					zend_string *str = php_stream_copy_to_mem(fp, entry.uncompressed_filesize, 0);
					if (str) {
						entry.uncompressed_filesize = ZSTR_LEN(str);
						actual_alias = estrndup(ZSTR_VAL(str), ZSTR_LEN(str));
						zend_string_release(str);
					} else {
						actual_alias = NULL;
						entry.uncompressed_filesize = 0;
					}
				}

				if (!entry.uncompressed_filesize || !actual_alias) {
					pefree(entry.filename, entry.is_persistent);
					PHAR_ZIP_FAIL("unable to read in alias, truncated");
				}

				php_stream_filter_flush(filter, 1);
				php_stream_filter_remove(filter, 1);
			} else {
				// TODO: refactor to avoid reallocation ???
//???			entry.uncompressed_filesize = php_stream_copy_to_mem(fp, &actual_alias, entry.uncompressed_filesize, 0)
				{
					zend_string *str = php_stream_copy_to_mem(fp, entry.uncompressed_filesize, 0);
					if (str) {
						entry.uncompressed_filesize = ZSTR_LEN(str);
						actual_alias = estrndup(ZSTR_VAL(str), ZSTR_LEN(str));
						zend_string_release(str);
					} else {
						actual_alias = NULL;
						entry.uncompressed_filesize = 0;
					}
				}

				if (!entry.uncompressed_filesize || !actual_alias) {
					pefree(entry.filename, entry.is_persistent);
					PHAR_ZIP_FAIL("unable to read in alias, truncated");
				}
			}

			/* return to central directory parsing */
			php_stream_seek(fp, saveloc, SEEK_SET);
		}

		phar_set_inode(&entry);
		zend_hash_str_add_mem(&mydata->manifest, entry.filename, entry.filename_len, (void *)&entry, sizeof(phar_entry_info));
	}

	mydata->fp = fp;

	if (zend_hash_str_exists(&(mydata->manifest), ".phar/stub.php", sizeof(".phar/stub.php")-1)) {
		mydata->is_data = 0;
	} else {
		mydata->is_data = 1;
	}

	zend_hash_str_add_ptr(&(PHAR_G(phar_fname_map)), mydata->fname, fname_len, mydata);

	if (actual_alias) {
		phar_archive_data *fd_ptr;

		if (!phar_validate_alias(actual_alias, mydata->alias_len)) {
			if (error) {
				spprintf(error, 4096, "phar error: invalid alias \"%s\" in zip-based phar \"%s\"", actual_alias, fname);
			}
			efree(actual_alias);
			zend_hash_str_del(&(PHAR_G(phar_fname_map)), mydata->fname, fname_len);
			return FAILURE;
		}

		mydata->is_temporary_alias = 0;

		if (NULL != (fd_ptr = zend_hash_str_find_ptr(&(PHAR_G(phar_alias_map)), actual_alias, mydata->alias_len))) {
			if (SUCCESS != phar_free_alias(fd_ptr, actual_alias, mydata->alias_len)) {
				if (error) {
					spprintf(error, 4096, "phar error: Unable to add zip-based phar \"%s\" with implicit alias, alias is already in use", fname);
				}
				efree(actual_alias);
				zend_hash_str_del(&(PHAR_G(phar_fname_map)), mydata->fname, fname_len);
				return FAILURE;
			}
		}

		mydata->alias = entry.is_persistent ? pestrndup(actual_alias, mydata->alias_len, 1) : actual_alias;

		if (entry.is_persistent) {
			efree(actual_alias);
		}

		zend_hash_str_add_ptr(&(PHAR_G(phar_alias_map)), actual_alias, mydata->alias_len, mydata);
	} else {
		phar_archive_data *fd_ptr;

		if (alias_len) {
			if (NULL != (fd_ptr = zend_hash_str_find_ptr(&(PHAR_G(phar_alias_map)), alias, alias_len))) {
				if (SUCCESS != phar_free_alias(fd_ptr, alias, alias_len)) {
					if (error) {
						spprintf(error, 4096, "phar error: Unable to add zip-based phar \"%s\" with explicit alias, alias is already in use", fname);
					}
					zend_hash_str_del(&(PHAR_G(phar_fname_map)), mydata->fname, fname_len);
					return FAILURE;
				}
			}

			zend_hash_str_add_ptr(&(PHAR_G(phar_alias_map)), actual_alias, mydata->alias_len, mydata);
			mydata->alias = pestrndup(alias, alias_len, mydata->is_persistent);
			mydata->alias_len = alias_len;
		} else {
			mydata->alias = pestrndup(mydata->fname, fname_len, mydata->is_persistent);
			mydata->alias_len = fname_len;
		}

		mydata->is_temporary_alias = 1;
	}

	if (pphar) {
		*pphar = mydata;
	}

	return SUCCESS;
}