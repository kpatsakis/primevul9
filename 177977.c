PHP_PHAR_API int phar_resolve_alias(char *alias, int alias_len, char **filename, int *filename_len) /* {{{ */ {
	phar_archive_data *fd_ptr;
	if (PHAR_G(phar_alias_map.u.flags)
			&& NULL != (fd_ptr = zend_hash_str_find_ptr(&(PHAR_G(phar_alias_map)), alias, alias_len))) {
		*filename = fd_ptr->fname;
		*filename_len = fd_ptr->fname_len;
		return SUCCESS;
	}
	return FAILURE;
}