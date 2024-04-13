int git_path_diriter_filename(
	const char **out,
	size_t *out_len,
	git_path_diriter *diriter)
{
	assert(out && out_len && diriter);

	assert(diriter->path.size > diriter->parent_len);

	*out = &diriter->path.ptr[diriter->parent_len+1];
	*out_len = diriter->path.size - diriter->parent_len - 1;
	return 0;
}