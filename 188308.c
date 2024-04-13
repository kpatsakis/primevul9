int git_path_diriter_init(
	git_path_diriter *diriter,
	const char *path,
	unsigned int flags)
{
	assert(diriter && path);

	memset(diriter, 0, sizeof(git_path_diriter));

	if (git_buf_puts(&diriter->path, path) < 0)
		return -1;

	git_path_trim_slashes(&diriter->path);

	if (diriter->path.size == 0) {
		git_error_set(GIT_ERROR_FILESYSTEM, "could not open directory '%s'", path);
		return -1;
	}

	if ((diriter->dir = opendir(diriter->path.ptr)) == NULL) {
		git_buf_dispose(&diriter->path);

		git_error_set(GIT_ERROR_OS, "failed to open directory '%s'", path);
		return -1;
	}

#ifdef GIT_USE_ICONV
	if ((flags & GIT_PATH_DIR_PRECOMPOSE_UNICODE) != 0)
		(void)git_path_iconv_init_precompose(&diriter->ic);
#endif

	diriter->parent_len = diriter->path.size;
	diriter->flags = flags;

	return 0;
}