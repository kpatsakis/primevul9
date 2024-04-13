zend_string *phar_find_in_include_path(char *filename, int filename_len, phar_archive_data **pphar) /* {{{ */
{
	zend_string *ret;
	char *path, *fname, *arch, *entry, *test;
	int arch_len, entry_len, fname_len;
	phar_archive_data *phar;

	if (pphar) {
		*pphar = NULL;
	} else {
		pphar = &phar;
	}

	if (!zend_is_executing() || !PHAR_G(cwd)) {
		return phar_save_resolve_path(filename, filename_len);
	}

	fname = (char*)zend_get_executed_filename();
	fname_len = strlen(fname);

	if (PHAR_G(last_phar) && !memcmp(fname, "phar://", 7) && fname_len - 7 >= PHAR_G(last_phar_name_len) && !memcmp(fname + 7, PHAR_G(last_phar_name), PHAR_G(last_phar_name_len))) {
		arch = estrndup(PHAR_G(last_phar_name), PHAR_G(last_phar_name_len));
		arch_len = PHAR_G(last_phar_name_len);
		phar = PHAR_G(last_phar);
		goto splitted;
	}

	if (fname_len < 7 || memcmp(fname, "phar://", 7) || SUCCESS != phar_split_fname(fname, strlen(fname), &arch, &arch_len, &entry, &entry_len, 1, 0)) {
		return phar_save_resolve_path(filename, filename_len);
	}

	efree(entry);

	if (*filename == '.') {
		int try_len;

		if (FAILURE == phar_get_archive(&phar, arch, arch_len, NULL, 0, NULL)) {
			efree(arch);
			return phar_save_resolve_path(filename, filename_len);
		}
splitted:
		if (pphar) {
			*pphar = phar;
		}

		try_len = filename_len;
		test = phar_fix_filepath(estrndup(filename, filename_len), &try_len, 1);

		if (*test == '/') {
			if (zend_hash_str_exists(&(phar->manifest), test + 1, try_len - 1)) {
				ret = strpprintf(0, "phar://%s%s", arch, test);
				efree(arch);
				efree(test);
				return ret;
			}
		} else {
			if (zend_hash_str_exists(&(phar->manifest), test, try_len)) {
				ret = strpprintf(0, "phar://%s/%s", arch, test);
				efree(arch);
				efree(test);
				return ret;
			}
		}
		efree(test);
	}

	spprintf(&path, MAXPATHLEN, "phar://%s/%s%c%s", arch, PHAR_G(cwd), DEFAULT_DIR_SEPARATOR, PG(include_path));
	efree(arch);
	ret = php_resolve_path(filename, filename_len, path);
	efree(path);

	if (ret && ZSTR_LEN(ret) > 8 && !strncmp(ZSTR_VAL(ret), "phar://", 7)) {
		/* found phar:// */
		if (SUCCESS != phar_split_fname(ZSTR_VAL(ret), ZSTR_LEN(ret), &arch, &arch_len, &entry, &entry_len, 1, 0)) {
			return ret;
		}

		*pphar = zend_hash_str_find_ptr(&(PHAR_G(phar_fname_map)), arch, arch_len);

		if (!*pphar && PHAR_G(manifest_cached)) {
			*pphar = zend_hash_str_find_ptr(&cached_phars, arch, arch_len);
		}

		efree(arch);
		efree(entry);
	}

	return ret;
}