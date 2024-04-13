CWD_API FILE *virtual_fopen(const char *path, const char *mode TSRMLS_DC) /* {{{ */
{
	cwd_state new_state;
	FILE *f;

	if (path[0] == '\0') { /* Fail to open empty path */
		return NULL;
	}

	CWD_STATE_COPY(&new_state, &CWDG(cwd));
	if (virtual_file_ex(&new_state, path, NULL, CWD_EXPAND TSRMLS_CC)) {
		CWD_STATE_FREE(&new_state);
		return NULL;
	}

	f = fopen(new_state.cwd, mode);

	CWD_STATE_FREE(&new_state);
	return f;
}