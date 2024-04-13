CWD_API int virtual_filepath(const char *path, char **filepath TSRMLS_DC) /* {{{ */
{
	return virtual_filepath_ex(path, filepath, php_is_file_ok TSRMLS_CC);
}