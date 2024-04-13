phar_entry_info *phar_get_entry_info_dir(phar_archive_data *phar, char *path, int path_len, char dir, char **error, int security) /* {{{ */
{
	const char *pcr_error;
	phar_entry_info *entry;
	int is_dir;

#ifdef PHP_WIN32
	phar_unixify_path_separators(path, path_len);
#endif

	is_dir = (path_len && (path[path_len - 1] == '/')) ? 1 : 0;

	if (error) {
		*error = NULL;
	}

	if (security && path_len >= sizeof(".phar")-1 && !memcmp(path, ".phar", sizeof(".phar")-1)) {
		if (error) {
			spprintf(error, 4096, "phar error: cannot directly access magic \".phar\" directory or files within it");
		}
		return NULL;
	}

	if (!path_len && !dir) {
		if (error) {
			spprintf(error, 4096, "phar error: invalid path \"%s\" must not be empty", path);
		}
		return NULL;
	}

	if (phar_path_check(&path, &path_len, &pcr_error) > pcr_is_ok) {
		if (error) {
			spprintf(error, 4096, "phar error: invalid path \"%s\" contains %s", path, pcr_error);
		}
		return NULL;
	}

	if (!phar->manifest.u.flags) {
		return NULL;
	}

	if (is_dir) {
		if (!path_len || path_len == 1) {
			return NULL;
		}
		path_len--;
	}

	if (NULL != (entry = zend_hash_str_find_ptr(&phar->manifest, path, path_len))) {
		if (entry->is_deleted) {
			/* entry is deleted, but has not been flushed to disk yet */
			return NULL;
		}
		if (entry->is_dir && !dir) {
			if (error) {
				spprintf(error, 4096, "phar error: path \"%s\" is a directory", path);
			}
			return NULL;
		}
		if (!entry->is_dir && dir == 2) {
			/* user requested a directory, we must return one */
			if (error) {
				spprintf(error, 4096, "phar error: path \"%s\" exists and is a not a directory", path);
			}
			return NULL;
		}
		return entry;
	}

	if (dir) {
		if (zend_hash_str_exists(&phar->virtual_dirs, path, path_len)) {
			/* a file or directory exists in a sub-directory of this path */
			entry = (phar_entry_info *) ecalloc(1, sizeof(phar_entry_info));
			/* this next line tells PharFileInfo->__destruct() to efree the filename */
			entry->is_temp_dir = entry->is_dir = 1;
			entry->filename = (char *) estrndup(path, path_len + 1);
			entry->filename_len = path_len;
			entry->phar = phar;
			return entry;
		}
	}

	if (phar->mounted_dirs.u.flags && zend_hash_num_elements(&phar->mounted_dirs)) {
		zend_string *str_key;

		ZEND_HASH_FOREACH_STR_KEY(&phar->mounted_dirs, str_key) {
			if ((int)ZSTR_LEN(str_key) >= path_len || strncmp(ZSTR_VAL(str_key), path, ZSTR_LEN(str_key))) {
				continue;
			} else {
				char *test;
				int test_len;
				php_stream_statbuf ssb;

				if (NULL == (entry = zend_hash_find_ptr(&phar->manifest, str_key))) {
					if (error) {
						spprintf(error, 4096, "phar internal error: mounted path \"%s\" could not be retrieved from manifest", ZSTR_VAL(str_key));
					}
					return NULL;
				}

				if (!entry->tmp || !entry->is_mounted) {
					if (error) {
						spprintf(error, 4096, "phar internal error: mounted path \"%s\" is not properly initialized as a mounted path", ZSTR_VAL(str_key));
					}
					return NULL;
				}

				test_len = spprintf(&test, MAXPATHLEN, "%s%s", entry->tmp, path + ZSTR_LEN(str_key));

				if (SUCCESS != php_stream_stat_path(test, &ssb)) {
					efree(test);
					return NULL;
				}

				if (ssb.sb.st_mode & S_IFDIR && !dir) {
					efree(test);
					if (error) {
						spprintf(error, 4096, "phar error: path \"%s\" is a directory", path);
					}
					return NULL;
				}

				if ((ssb.sb.st_mode & S_IFDIR) == 0 && dir) {
					efree(test);
					/* user requested a directory, we must return one */
					if (error) {
						spprintf(error, 4096, "phar error: path \"%s\" exists and is a not a directory", path);
					}
					return NULL;
				}

				/* mount the file just in time */
				if (SUCCESS != phar_mount_entry(phar, test, test_len, path, path_len)) {
					efree(test);
					if (error) {
						spprintf(error, 4096, "phar error: path \"%s\" exists as file \"%s\" and could not be mounted", path, test);
					}
					return NULL;
				}

				efree(test);

				if (NULL == (entry = zend_hash_str_find_ptr(&phar->manifest, path, path_len))) {
					if (error) {
						spprintf(error, 4096, "phar error: path \"%s\" exists as file \"%s\" and could not be retrieved after being mounted", path, test);
					}
					return NULL;
				}
				return entry;
			}
		} ZEND_HASH_FOREACH_END();
	}

	return NULL;
}