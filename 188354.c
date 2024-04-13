bool git_path_islink(const char *path)
{
	struct stat st;

	assert(path);
	if (p_lstat(path, &st) < 0)
		return false;

	return S_ISLNK(st.st_mode) != 0;
}