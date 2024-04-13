CWD_API int virtual_utime(const char *filename, struct utimbuf *buf TSRMLS_DC) /* {{{ */
{
	cwd_state new_state;
	int ret;

	CWD_STATE_COPY(&new_state, &CWDG(cwd));
	if (virtual_file_ex(&new_state, filename, NULL, CWD_REALPATH TSRMLS_CC)) {
		CWD_STATE_FREE(&new_state);
		return -1;
	}

#ifdef TSRM_WIN32
	ret = win32_utime(new_state.cwd, buf);
#else
	ret = utime(new_state.cwd, buf);
#endif

	CWD_STATE_FREE(&new_state);
	return ret;
}