int git_path_diriter_init(
	git_path_diriter *diriter,
	const char *path,
	unsigned int flags)
{
	git_win32_path path_filter;

	static int is_win7_or_later = -1;
	if (is_win7_or_later < 0)
		is_win7_or_later = git_has_win32_version(6, 1, 0);

	assert(diriter && path);

	memset(diriter, 0, sizeof(git_path_diriter));
	diriter->handle = INVALID_HANDLE_VALUE;

	if (git_buf_puts(&diriter->path_utf8, path) < 0)
		return -1;

	git_path_trim_slashes(&diriter->path_utf8);

	if (diriter->path_utf8.size == 0) {
		git_error_set(GIT_ERROR_FILESYSTEM, "could not open directory '%s'", path);
		return -1;
	}

	if ((diriter->parent_len = git_win32_path_from_utf8(diriter->path, diriter->path_utf8.ptr)) < 0 ||
			!git_win32__findfirstfile_filter(path_filter, diriter->path_utf8.ptr)) {
		git_error_set(GIT_ERROR_OS, "could not parse the directory path '%s'", path);
		return -1;
	}

	diriter->handle = FindFirstFileExW(
		path_filter,
		is_win7_or_later ? FindExInfoBasic : FindExInfoStandard,
		&diriter->current,
		FindExSearchNameMatch,
		NULL,
		is_win7_or_later ? FIND_FIRST_EX_LARGE_FETCH : 0);

	if (diriter->handle == INVALID_HANDLE_VALUE) {
		git_error_set(GIT_ERROR_OS, "could not open directory '%s'", path);
		return -1;
	}

	diriter->parent_utf8_len = diriter->path_utf8.size;
	diriter->flags = flags;
	return 0;
}