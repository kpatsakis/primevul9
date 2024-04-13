int git_path_diriter_fullpath(
	const char **out,
	size_t *out_len,
	git_path_diriter *diriter)
{
	assert(out && out_len && diriter);

	*out = diriter->path_utf8.ptr;
	*out_len = diriter->path_utf8.size;
	return 0;
}