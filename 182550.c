CWD_API int virtual_stat(const char *path, struct stat *buf TSRMLS_DC) /* {{{ */
{
	cwd_state new_state;
	int retval;

	CWD_STATE_COPY(&new_state, &CWDG(cwd));
	if (virtual_file_ex(&new_state, path, NULL, CWD_REALPATH TSRMLS_CC)) {
		CWD_STATE_FREE(&new_state);
		return -1;
	}

	retval = php_sys_stat(new_state.cwd, buf);

	CWD_STATE_FREE(&new_state);
	return retval;
}