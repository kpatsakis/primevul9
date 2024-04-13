int git_path_join_unrooted(
	git_buf *path_out, const char *path, const char *base, ssize_t *root_at)
{
	ssize_t root;

	assert(path && path_out);

	root = (ssize_t)git_path_root(path);

	if (base != NULL && root < 0) {
		if (git_buf_joinpath(path_out, base, path) < 0)
			return -1;

		root = (ssize_t)strlen(base);
	} else {
		if (git_buf_sets(path_out, path) < 0)
			return -1;

		if (root < 0)
			root = 0;
		else if (base)
			git_path_equal_or_prefixed(base, path, &root);
	}

	if (root_at)
		*root_at = root;

	return 0;
}