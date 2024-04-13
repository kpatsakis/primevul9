int git_path_prettify(git_buf *path_out, const char *path, const char *base)
{
	char buf[GIT_PATH_MAX];

	assert(path && path_out);

	/* construct path if needed */
	if (base != NULL && git_path_root(path) < 0) {
		if (git_buf_joinpath(path_out, base, path) < 0)
			return -1;
		path = path_out->ptr;
	}

	if (p_realpath(path, buf) == NULL) {
		/* git_error_set resets the errno when dealing with a GIT_ERROR_OS kind of error */
		int error = (errno == ENOENT || errno == ENOTDIR) ? GIT_ENOTFOUND : -1;
		git_error_set(GIT_ERROR_OS, "failed to resolve path '%s'", path);

		git_buf_clear(path_out);

		return error;
	}

	return git_buf_sets(path_out, buf);
}