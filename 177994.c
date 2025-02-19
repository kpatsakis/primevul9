void phar_add_virtual_dirs(phar_archive_data *phar, char *filename, int filename_len) /* {{{ */
{
	const char *s;

	while ((s = zend_memrchr(filename, '/', filename_len))) {
		filename_len = s - filename;
		if (!filename_len || NULL == zend_hash_str_add_empty_element(&phar->virtual_dirs, filename, filename_len)) {
			break;
		}
	}
}