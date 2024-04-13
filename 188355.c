int git_path_normalize_slashes(git_buf *out, const char *path)
{
	int error;
	char *p;

	if ((error = git_buf_puts(out, path)) < 0)
		return error;

	for (p = out->ptr; *p; p++) {
		if (*p == '\\')
			*p = '/';
	}

	return 0;
}