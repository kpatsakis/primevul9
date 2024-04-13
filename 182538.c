CWD_API int virtual_creat(const char *path, mode_t mode TSRMLS_DC) /* {{{ */
{
	cwd_state new_state;
	int f;

	CWD_STATE_COPY(&new_state, &CWDG(cwd));
	if (virtual_file_ex(&new_state, path, NULL, CWD_FILEPATH TSRMLS_CC)) {
		CWD_STATE_FREE(&new_state);
		return -1;
	}

	f = creat(new_state.cwd,  mode);

	CWD_STATE_FREE(&new_state);
	return f;
}