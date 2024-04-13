CWD_API int virtual_rmdir(const char *pathname TSRMLS_DC) /* {{{ */
{
	cwd_state new_state;
	int retval;

	CWD_STATE_COPY(&new_state, &CWDG(cwd));
	if (virtual_file_ex(&new_state, pathname, NULL, CWD_EXPAND TSRMLS_CC)) {
		CWD_STATE_FREE(&new_state);
		return -1;
	}

	retval = rmdir(new_state.cwd);

	CWD_STATE_FREE(&new_state);
	return retval;
}