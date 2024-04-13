bool git_path_exists(const char *path)
{
	assert(path);
	return p_access(path, F_OK) == 0;
}