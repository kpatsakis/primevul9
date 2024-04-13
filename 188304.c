int git_path_diriter_stat(struct stat *out, git_path_diriter *diriter)
{
	assert(out && diriter);

	return git_path_lstat(diriter->path.ptr, out);
}