CWD_API char *tsrm_realpath(const char *path, char *real_path TSRMLS_DC) /* {{{ */
{
	cwd_state new_state;
	char cwd[MAXPATHLEN];

	/* realpath("") returns CWD */
	if (!*path) {
		new_state.cwd = (char*)malloc(1);
		if (new_state.cwd == NULL) {
			return NULL;
		}
		new_state.cwd[0] = '\0';
		new_state.cwd_length = 0;
		if (VCWD_GETCWD(cwd, MAXPATHLEN)) {
			path = cwd;
		}
	} else if (!IS_ABSOLUTE_PATH(path, strlen(path)) &&
					VCWD_GETCWD(cwd, MAXPATHLEN)) {
		new_state.cwd = strdup(cwd);
		new_state.cwd_length = strlen(cwd);
	} else {
		new_state.cwd = (char*)malloc(1);
		if (new_state.cwd == NULL) {
			return NULL;
		}
		new_state.cwd[0] = '\0';
		new_state.cwd_length = 0;
	}

	if (virtual_file_ex(&new_state, path, NULL, CWD_REALPATH TSRMLS_CC)) {
		free(new_state.cwd);
		return NULL;
	}

	if (real_path) {
		int copy_len = new_state.cwd_length>MAXPATHLEN-1 ? MAXPATHLEN-1 : new_state.cwd_length;
		memcpy(real_path, new_state.cwd, copy_len);
		real_path[copy_len] = '\0';
		free(new_state.cwd);
		return real_path;
	} else {
		return new_state.cwd;
	}
}