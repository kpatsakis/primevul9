CWD_API char *virtual_realpath(const char *path, char *real_path TSRMLS_DC) /* {{{ */
{
	cwd_state new_state;
	char *retval;
	char cwd[MAXPATHLEN];

	/* realpath("") returns CWD */
	if (!*path) {
		new_state.cwd = (char*)malloc(1);
		if (new_state.cwd == NULL) {
			retval = NULL;
			goto end;
		}
		new_state.cwd[0] = '\0';
		new_state.cwd_length = 0;
		if (VCWD_GETCWD(cwd, MAXPATHLEN)) {
			path = cwd;
		}
	} else if (!IS_ABSOLUTE_PATH(path, strlen(path))) {
		CWD_STATE_COPY(&new_state, &CWDG(cwd));
	} else {
		new_state.cwd = (char*)malloc(1);
		if (new_state.cwd == NULL) {
			retval = NULL;
			goto end;
		}
		new_state.cwd[0] = '\0';
		new_state.cwd_length = 0;
	}

	if (virtual_file_ex(&new_state, path, NULL, CWD_REALPATH TSRMLS_CC)==0) {
		int len = new_state.cwd_length>MAXPATHLEN-1?MAXPATHLEN-1:new_state.cwd_length;

		memcpy(real_path, new_state.cwd, len);
		real_path[len] = '\0';
		retval = real_path;
	} else {
		retval = NULL;
	}

	CWD_STATE_FREE(&new_state);
end:
	return retval;
}