int git_path_diriter_next(git_path_diriter *diriter)
{
	struct dirent *de;
	const char *filename;
	size_t filename_len;
	bool skip_dot = !(diriter->flags & GIT_PATH_DIR_INCLUDE_DOT_AND_DOTDOT);
	int error = 0;

	assert(diriter);

	errno = 0;

	do {
		if ((de = readdir(diriter->dir)) == NULL) {
			if (!errno)
				return GIT_ITEROVER;

			git_error_set(GIT_ERROR_OS,
				"could not read directory '%s'", diriter->path.ptr);
			return -1;
		}
	} while (skip_dot && git_path_is_dot_or_dotdot(de->d_name));

	filename = de->d_name;
	filename_len = strlen(filename);

#ifdef GIT_USE_ICONV
	if ((diriter->flags & GIT_PATH_DIR_PRECOMPOSE_UNICODE) != 0 &&
		(error = git_path_iconv(&diriter->ic, &filename, &filename_len)) < 0)
		return error;
#endif

	git_buf_truncate(&diriter->path, diriter->parent_len);

	if (diriter->parent_len > 0 &&
		diriter->path.ptr[diriter->parent_len-1] != '/')
		git_buf_putc(&diriter->path, '/');

	git_buf_put(&diriter->path, filename, filename_len);

	if (git_buf_oom(&diriter->path))
		return -1;

	return error;
}