void git_path_diriter_free(git_path_diriter *diriter)
{
	if (diriter == NULL)
		return;

	git_buf_dispose(&diriter->path_utf8);

	if (diriter->handle != INVALID_HANDLE_VALUE) {
		FindClose(diriter->handle);
		diriter->handle = INVALID_HANDLE_VALUE;
	}
}