bool git_path_isfile(const char *path)
{
	struct stat st;

	assert(path);
	if (p_stat(path, &st) < 0)
		return false;

	return S_ISREG(st.st_mode) != 0;
}