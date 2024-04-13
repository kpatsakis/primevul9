int git_path_walk_up(
	git_buf *path,
	const char *ceiling,
	int (*cb)(void *data, const char *),
	void *data)
{
	int error = 0;
	git_buf iter;
	ssize_t stop = 0, scan;
	char oldc = '\0';

	assert(path && cb);

	if (ceiling != NULL) {
		if (git__prefixcmp(path->ptr, ceiling) == 0)
			stop = (ssize_t)strlen(ceiling);
		else
			stop = git_buf_len(path);
	}
	scan = git_buf_len(path);

	/* empty path: yield only once */
	if (!scan) {
		error = cb(data, "");
		if (error)
			git_error_set_after_callback(error);
		return error;
	}

	iter.ptr = path->ptr;
	iter.size = git_buf_len(path);
	iter.asize = path->asize;

	while (scan >= stop) {
		error = cb(data, iter.ptr);
		iter.ptr[scan] = oldc;

		if (error) {
			git_error_set_after_callback(error);
			break;
		}

		scan = git_buf_rfind_next(&iter, '/');
		if (scan >= 0) {
			scan++;
			oldc = iter.ptr[scan];
			iter.size = scan;
			iter.ptr[scan] = '\0';
		}
	}

	if (scan >= 0)
		iter.ptr[scan] = oldc;

	/* relative path: yield for the last component */
	if (!error && stop == 0 && iter.ptr[0] != '/') {
		error = cb(data, "");
		if (error)
			git_error_set_after_callback(error);
	}

	return error;
}