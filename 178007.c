phar_entry_data *phar_get_or_create_entry_data(char *fname, int fname_len, char *path, int path_len, const char *mode, char allow_dir, char **error, int security) /* {{{ */
{
	phar_archive_data *phar;
	phar_entry_info *entry, etemp;
	phar_entry_data *ret;
	const char *pcr_error;
	char is_dir;

#ifdef PHP_WIN32
	phar_unixify_path_separators(path, path_len);
#endif

	is_dir = (path_len && path[path_len - 1] == '/') ? 1 : 0;

	if (FAILURE == phar_get_archive(&phar, fname, fname_len, NULL, 0, error)) {
		return NULL;
	}

	if (FAILURE == phar_get_entry_data(&ret, fname, fname_len, path, path_len, mode, allow_dir, error, security)) {
		return NULL;
	} else if (ret) {
		return ret;
	}

	if (phar_path_check(&path, &path_len, &pcr_error) > pcr_is_ok) {
		if (error) {
			spprintf(error, 0, "phar error: invalid path \"%s\" contains %s", path, pcr_error);
		}
		return NULL;
	}

	if (phar->is_persistent && FAILURE == phar_copy_on_write(&phar)) {
		if (error) {
			spprintf(error, 4096, "phar error: file \"%s\" in phar \"%s\" cannot be created, could not make cached phar writeable", path, fname);
		}
		return NULL;
	}

	/* create a new phar data holder */
	ret = (phar_entry_data *) emalloc(sizeof(phar_entry_data));

	/* create an entry, this is a new file */
	memset(&etemp, 0, sizeof(phar_entry_info));
	etemp.filename_len = path_len;
	etemp.fp_type = PHAR_MOD;
	etemp.fp = php_stream_fopen_tmpfile();

	if (!etemp.fp) {
		if (error) {
			spprintf(error, 0, "phar error: unable to create temporary file");
		}
		efree(ret);
		return NULL;
	}

	etemp.fp_refcount = 1;

	if (allow_dir == 2) {
		etemp.is_dir = 1;
		etemp.flags = etemp.old_flags = PHAR_ENT_PERM_DEF_DIR;
	} else {
		etemp.flags = etemp.old_flags = PHAR_ENT_PERM_DEF_FILE;
	}
	if (is_dir) {
		etemp.filename_len--; /* strip trailing / */
		path_len--;
	}

	phar_add_virtual_dirs(phar, path, path_len);
	etemp.is_modified = 1;
	etemp.timestamp = time(0);
	etemp.is_crc_checked = 1;
	etemp.phar = phar;
	etemp.filename = estrndup(path, path_len);
	etemp.is_zip = phar->is_zip;

	if (phar->is_tar) {
		etemp.is_tar = phar->is_tar;
		etemp.tar_type = etemp.is_dir ? TAR_DIR : TAR_FILE;
	}

	if (NULL == (entry = zend_hash_str_add_mem(&phar->manifest, etemp.filename, path_len, (void*)&etemp, sizeof(phar_entry_info)))) {
		php_stream_close(etemp.fp);
		if (error) {
			spprintf(error, 0, "phar error: unable to add new entry \"%s\" to phar \"%s\"", etemp.filename, phar->fname);
		}
		efree(ret);
		efree(etemp.filename);
		return NULL;
	}

	if (!entry) {
		php_stream_close(etemp.fp);
		efree(etemp.filename);
		efree(ret);
		return NULL;
	}

	++(phar->refcount);
	ret->phar = phar;
	ret->fp = entry->fp;
	ret->position = ret->zero = 0;
	ret->for_write = 1;
	ret->is_zip = entry->is_zip;
	ret->is_tar = entry->is_tar;
	ret->internal_file = entry;

	return ret;
}