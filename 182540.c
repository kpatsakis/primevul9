CWD_API int virtual_open(const char *path TSRMLS_DC, int flags, ...) /* {{{ */
{
	cwd_state new_state;
	int f;

	CWD_STATE_COPY(&new_state, &CWDG(cwd));
	if (virtual_file_ex(&new_state, path, NULL, CWD_FILEPATH TSRMLS_CC)) {
		CWD_STATE_FREE(&new_state);
		return -1;
	}

	if (flags & O_CREAT) {
		mode_t mode;
		va_list arg;

		va_start(arg, flags);
		mode = (mode_t) va_arg(arg, int);
		va_end(arg);

		f = open(new_state.cwd, flags, mode);
	} else {
		f = open(new_state.cwd, flags);
	}
	CWD_STATE_FREE(&new_state);
	return f;
}