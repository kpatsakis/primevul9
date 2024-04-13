bool git_path_contains_file(git_buf *base, const char *file)
{
	return _check_dir_contents(base, file, &git_path_isfile);
}