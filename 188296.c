int git_path_dirload(
	git_vector *contents,
	const char *path,
	size_t prefix_len,
	uint32_t flags)
{
	git_path_diriter iter = GIT_PATH_DIRITER_INIT;
	const char *name;
	size_t name_len;
	char *dup;
	int error;

	assert(contents && path);

	if ((error = git_path_diriter_init(&iter, path, flags)) < 0)
		return error;

	while ((error = git_path_diriter_next(&iter)) == 0) {
		if ((error = git_path_diriter_fullpath(&name, &name_len, &iter)) < 0)
			break;

		assert(name_len > prefix_len);

		dup = git__strndup(name + prefix_len, name_len - prefix_len);
		GIT_ERROR_CHECK_ALLOC(dup);

		if ((error = git_vector_insert(contents, dup)) < 0)
			break;
	}

	if (error == GIT_ITEROVER)
		error = 0;

	git_path_diriter_free(&iter);
	return error;
}