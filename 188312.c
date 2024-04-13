int git_path_diriter_stat(struct stat *out, git_path_diriter *diriter)
{
	assert(out && diriter);

	return git_win32__file_attribute_to_stat(out,
		(WIN32_FILE_ATTRIBUTE_DATA *)&diriter->current,
		diriter->path);
}