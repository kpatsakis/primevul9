bool git_path_contains_dir(git_buf *base, const char *subdir)
{
	return _check_dir_contents(base, subdir, &git_path_isdir);
}