int git_path_diriter_filename(
	const char **out,
	size_t *out_len,
	git_path_diriter *diriter)
{
	assert(out && out_len && diriter);

	assert(diriter->path_utf8.size > diriter->parent_utf8_len);

	*out = &diriter->path_utf8.ptr[diriter->parent_utf8_len+1];
	*out_len = diriter->path_utf8.size - diriter->parent_utf8_len - 1;
	return 0;
}