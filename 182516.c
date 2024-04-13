CWD_API int virtual_access(const char *pathname, int mode TSRMLS_DC) /* {{{ */
{
	cwd_state new_state;
	int ret;

	CWD_STATE_COPY(&new_state, &CWDG(cwd));
	if (virtual_file_ex(&new_state, pathname, NULL, CWD_REALPATH TSRMLS_CC)) {
		CWD_STATE_FREE(&new_state);
		return -1;
	}

#if defined(TSRM_WIN32)
	ret = tsrm_win32_access(new_state.cwd, mode TSRMLS_CC);
#else
	ret = access(new_state.cwd, mode);
#endif

	CWD_STATE_FREE(&new_state);

	return ret;
}