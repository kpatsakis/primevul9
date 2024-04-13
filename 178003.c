int phar_get_archive(phar_archive_data **archive, char *fname, int fname_len, char *alias, int alias_len, char **error) /* {{{ */
{
	phar_archive_data *fd, *fd_ptr;
	char *my_realpath, *save;
	int save_len;

	phar_request_initialize();

	if (error) {
		*error = NULL;
	}

	*archive = NULL;

	if (PHAR_G(last_phar) && fname_len == PHAR_G(last_phar_name_len) && !memcmp(fname, PHAR_G(last_phar_name), fname_len)) {
		*archive = PHAR_G(last_phar);
		if (alias && alias_len) {

			if (!PHAR_G(last_phar)->is_temporary_alias && (alias_len != PHAR_G(last_phar)->alias_len || memcmp(PHAR_G(last_phar)->alias, alias, alias_len))) {
				if (error) {
					spprintf(error, 0, "alias \"%s\" is already used for archive \"%s\" cannot be overloaded with \"%s\"", alias, PHAR_G(last_phar)->fname, fname);
				}
				*archive = NULL;
				return FAILURE;
			}

			if (PHAR_G(last_phar)->alias_len && NULL != (fd_ptr = zend_hash_str_find_ptr(&(PHAR_G(phar_alias_map)), PHAR_G(last_phar)->alias, PHAR_G(last_phar)->alias_len))) {
				zend_hash_str_del(&(PHAR_G(phar_alias_map)), PHAR_G(last_phar)->alias, PHAR_G(last_phar)->alias_len);
			}

			zend_hash_str_add_ptr(&(PHAR_G(phar_alias_map)), alias, alias_len, *archive);
			PHAR_G(last_alias) = alias;
			PHAR_G(last_alias_len) = alias_len;
		}

		return SUCCESS;
	}

	if (alias && alias_len && PHAR_G(last_phar) && alias_len == PHAR_G(last_alias_len) && !memcmp(alias, PHAR_G(last_alias), alias_len)) {
		fd = PHAR_G(last_phar);
		fd_ptr = fd;
		goto alias_success;
	}

	if (alias && alias_len) {
		if (NULL != (fd_ptr = zend_hash_str_find_ptr(&(PHAR_G(phar_alias_map)), alias, alias_len))) {
alias_success:
			if (fname && (fname_len != fd_ptr->fname_len || strncmp(fname, fd_ptr->fname, fname_len))) {
				if (error) {
					spprintf(error, 0, "alias \"%s\" is already used for archive \"%s\" cannot be overloaded with \"%s\"", alias, fd_ptr->fname, fname);
				}
				if (SUCCESS == phar_free_alias(fd_ptr, alias, alias_len)) {
					if (error) {
						efree(*error);
						*error = NULL;
					}
				}
				return FAILURE;
			}

			*archive = fd_ptr;
			fd = fd_ptr;
			PHAR_G(last_phar) = fd;
			PHAR_G(last_phar_name) = fd->fname;
			PHAR_G(last_phar_name_len) = fd->fname_len;
			PHAR_G(last_alias) = alias;
			PHAR_G(last_alias_len) = alias_len;

			return SUCCESS;
		}

		if (PHAR_G(manifest_cached) && NULL != (fd_ptr = zend_hash_str_find_ptr(&cached_alias, alias, alias_len))) {
			goto alias_success;
		}
	}

	my_realpath = NULL;
	save = fname;
	save_len = fname_len;

	if (fname && fname_len) {
		if (NULL != (fd_ptr = zend_hash_str_find_ptr(&(PHAR_G(phar_fname_map)), fname, fname_len))) {
			*archive = fd_ptr;
			fd = fd_ptr;

			if (alias && alias_len) {
				if (!fd->is_temporary_alias && (alias_len != fd->alias_len || memcmp(fd->alias, alias, alias_len))) {
					if (error) {
						spprintf(error, 0, "alias \"%s\" is already used for archive \"%s\" cannot be overloaded with \"%s\"", alias, fd_ptr->fname, fname);
					}
					return FAILURE;
				}

				if (fd->alias_len && NULL != (fd_ptr = zend_hash_str_find_ptr(&(PHAR_G(phar_alias_map)), fd->alias, fd->alias_len))) {
					zend_hash_str_del(&(PHAR_G(phar_alias_map)), fd->alias, fd->alias_len);
				}

				zend_hash_str_add_ptr(&(PHAR_G(phar_alias_map)), alias, alias_len, fd);
			}

			PHAR_G(last_phar) = fd;
			PHAR_G(last_phar_name) = fd->fname;
			PHAR_G(last_phar_name_len) = fd->fname_len;
			PHAR_G(last_alias) = fd->alias;
			PHAR_G(last_alias_len) = fd->alias_len;

			return SUCCESS;
		}

		if (PHAR_G(manifest_cached) && NULL != (fd_ptr = zend_hash_str_find_ptr(&cached_phars, fname, fname_len))) {
			*archive = fd_ptr;
			fd = fd_ptr;

			/* this could be problematic - alias should never be different from manifest alias
			   for cached phars */
			if (!fd->is_temporary_alias && alias && alias_len) {
				if (alias_len != fd->alias_len || memcmp(fd->alias, alias, alias_len)) {
					if (error) {
						spprintf(error, 0, "alias \"%s\" is already used for archive \"%s\" cannot be overloaded with \"%s\"", alias, fd_ptr->fname, fname);
					}
					return FAILURE;
				}
			}

			PHAR_G(last_phar) = fd;
			PHAR_G(last_phar_name) = fd->fname;
			PHAR_G(last_phar_name_len) = fd->fname_len;
			PHAR_G(last_alias) = fd->alias;
			PHAR_G(last_alias_len) = fd->alias_len;

			return SUCCESS;
		}

		if (NULL != (fd_ptr = zend_hash_str_find_ptr(&(PHAR_G(phar_alias_map)), save, save_len))) {
			fd = *archive = fd_ptr;

			PHAR_G(last_phar) = fd;
			PHAR_G(last_phar_name) = fd->fname;
			PHAR_G(last_phar_name_len) = fd->fname_len;
			PHAR_G(last_alias) = fd->alias;
			PHAR_G(last_alias_len) = fd->alias_len;

			return SUCCESS;
		}

		if (PHAR_G(manifest_cached) && NULL != (fd_ptr = zend_hash_str_find_ptr(&cached_alias, save, save_len))) {
			fd = *archive = fd_ptr;

			PHAR_G(last_phar) = fd;
			PHAR_G(last_phar_name) = fd->fname;
			PHAR_G(last_phar_name_len) = fd->fname_len;
			PHAR_G(last_alias) = fd->alias;
			PHAR_G(last_alias_len) = fd->alias_len;

			return SUCCESS;
		}

		/* not found, try converting \ to / */
		my_realpath = expand_filepath(fname, my_realpath);

		if (my_realpath) {
			fname_len = strlen(my_realpath);
			fname = my_realpath;
		} else {
			return FAILURE;
		}
#ifdef PHP_WIN32
		phar_unixify_path_separators(fname, fname_len);
#endif

		if (NULL != (fd_ptr = zend_hash_str_find_ptr(&(PHAR_G(phar_fname_map)), fname, fname_len))) {
realpath_success:
			*archive = fd_ptr;
			fd = fd_ptr;

			if (alias && alias_len) {
				zend_hash_str_add_ptr(&(PHAR_G(phar_alias_map)), alias, alias_len, fd);
			}

			efree(my_realpath);

			PHAR_G(last_phar) = fd;
			PHAR_G(last_phar_name) = fd->fname;
			PHAR_G(last_phar_name_len) = fd->fname_len;
			PHAR_G(last_alias) = fd->alias;
			PHAR_G(last_alias_len) = fd->alias_len;

			return SUCCESS;
		}

		if (PHAR_G(manifest_cached) && NULL != (fd_ptr = zend_hash_str_find_ptr(&cached_phars, fname, fname_len))) {
			goto realpath_success;
		}

		efree(my_realpath);
	}

	return FAILURE;
}