static int diriter_update_paths(git_path_diriter *diriter)
{
	size_t filename_len, path_len;

	filename_len = wcslen(diriter->current.cFileName);

	if (GIT_ADD_SIZET_OVERFLOW(&path_len, diriter->parent_len, filename_len) ||
		GIT_ADD_SIZET_OVERFLOW(&path_len, path_len, 2))
		return -1;

	if (path_len > GIT_WIN_PATH_UTF16) {
		git_error_set(GIT_ERROR_FILESYSTEM,
			"invalid path '%.*ls\\%ls' (path too long)",
			diriter->parent_len, diriter->path, diriter->current.cFileName);
		return -1;
	}

	diriter->path[diriter->parent_len] = L'\\';
	memcpy(&diriter->path[diriter->parent_len+1],
		diriter->current.cFileName, filename_len * sizeof(wchar_t));
	diriter->path[path_len-1] = L'\0';

	git_buf_truncate(&diriter->path_utf8, diriter->parent_utf8_len);

	if (diriter->parent_utf8_len > 0 &&
		diriter->path_utf8.ptr[diriter->parent_utf8_len-1] != '/')
		git_buf_putc(&diriter->path_utf8, '/');

	git_buf_put_w(&diriter->path_utf8, diriter->current.cFileName, filename_len);

	if (git_buf_oom(&diriter->path_utf8))
		return -1;

	return 0;
}