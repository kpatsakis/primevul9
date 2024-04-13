CWD_API int virtual_mkdir(const char *pathname, mode_t mode TSRMLS_DC) /* {{{ */
{
	cwd_state new_state;
	int retval;

	CWD_STATE_COPY(&new_state, &CWDG(cwd));
	if (virtual_file_ex(&new_state, pathname, NULL, CWD_FILEPATH TSRMLS_CC)) {
		CWD_STATE_FREE(&new_state);
		return -1;
	}

#ifdef TSRM_WIN32
	retval = mkdir(new_state.cwd);
#else
	retval = mkdir(new_state.cwd, mode);
#endif
	CWD_STATE_FREE(&new_state);
	return retval;
}