bool git_path_contains(git_buf *dir, const char *item)
{
	return _check_dir_contents(dir, item, &git_path_exists);
}