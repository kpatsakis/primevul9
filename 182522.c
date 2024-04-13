CWD_API int virtual_chdir(const char *path TSRMLS_DC) /* {{{ */
{
	return virtual_file_ex(&CWDG(cwd), path, php_is_dir_ok, CWD_REALPATH TSRMLS_CC)?-1:0;
}