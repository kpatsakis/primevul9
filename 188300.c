int git_path_lstat(const char *path, struct stat *st)
{
	if (p_lstat(path, st) == 0)
		return 0;

	return git_path_set_error(errno, path, "stat");
}