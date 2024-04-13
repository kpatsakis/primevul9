bool git_path_isdir(const char *path)
{
	struct stat st;
	if (p_stat(path, &st) < 0)
		return false;

	return S_ISDIR(st.st_mode) != 0;
}