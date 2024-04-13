int git_path_diriter_fullpath(
	const char **out,
	size_t *out_len,
	git_path_diriter *diriter)
{
	assert(out && out_len && diriter);

	*out = diriter->path.ptr;
	*out_len = diriter->path.size;
	return 0;
}