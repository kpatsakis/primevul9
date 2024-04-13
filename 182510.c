CWD_API int virtual_chmod(const char *filename, mode_t mode TSRMLS_DC) /* {{{ */
{
	cwd_state new_state;
	int ret;

	CWD_STATE_COPY(&new_state, &CWDG(cwd));
	if (virtual_file_ex(&new_state, filename, NULL, CWD_REALPATH TSRMLS_CC)) {
		CWD_STATE_FREE(&new_state);
		return -1;
	}

	ret = chmod(new_state.cwd, mode);

	CWD_STATE_FREE(&new_state);
	return ret;
}