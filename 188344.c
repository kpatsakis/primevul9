bool git_path_is_empty_dir(const char *path)
{
	int error;
	git_buf dir = GIT_BUF_INIT;

	if (!git_path_isdir(path))
		return false;

	if ((error = git_buf_sets(&dir, path)) != 0)
		git_error_clear();
	else
		error = git_path_direach(&dir, 0, path_found_entry, NULL);

	git_buf_dispose(&dir);

	return !error;
}