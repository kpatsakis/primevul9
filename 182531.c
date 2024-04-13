CWD_API DIR *virtual_opendir(const char *pathname TSRMLS_DC) /* {{{ */
{
	cwd_state new_state;
	DIR *retval;

	CWD_STATE_COPY(&new_state, &CWDG(cwd));
	if (virtual_file_ex(&new_state, pathname, NULL, CWD_REALPATH TSRMLS_CC)) {
		CWD_STATE_FREE(&new_state);
		return NULL;
	}

	retval = opendir(new_state.cwd);

	CWD_STATE_FREE(&new_state);
	return retval;
}