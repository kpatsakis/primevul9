const char *prefix_path(const char *prefix, int len, const char *path)
{
	const char *orig = path;
	char *sanitized = xmalloc(len + strlen(path) + 1);
	if (is_absolute_path(orig))
		strcpy(sanitized, path);
	else {
		if (len)
			memcpy(sanitized, prefix, len);
		strcpy(sanitized + len, path);
	}
	if (normalize_path_copy(sanitized, sanitized))
		goto error_out;
	if (is_absolute_path(orig)) {
		size_t root_len, len, total;
		const char *work_tree = get_git_work_tree();
		if (!work_tree)
			goto error_out;
		len = strlen(work_tree);
		root_len = offset_1st_component(work_tree);
		total = strlen(sanitized) + 1;
		if (strncmp(sanitized, work_tree, len) ||
		    (len > root_len && sanitized[len] != '\0' && sanitized[len] != '/')) {
		error_out:
			die("'%s' is outside repository", orig);
		}
		if (sanitized[len] == '/')
			len++;
		memmove(sanitized, sanitized + len, total - len);
	}
	return sanitized;
}