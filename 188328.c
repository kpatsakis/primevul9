int git_path_resolve_relative(git_buf *path, size_t ceiling)
{
	char *base, *to, *from, *next;
	size_t len;

	GIT_ERROR_CHECK_ALLOC_BUF(path);

	if (ceiling > path->size)
		ceiling = path->size;

	/* recognize drive prefixes, etc. that should not be backed over */
	if (ceiling == 0)
		ceiling = git_path_root(path->ptr) + 1;

	/* recognize URL prefixes that should not be backed over */
	if (ceiling == 0) {
		for (next = path->ptr; *next && git__isalpha(*next); ++next);
		if (next[0] == ':' && next[1] == '/' && next[2] == '/')
			ceiling = (next + 3) - path->ptr;
	}

	base = to = from = path->ptr + ceiling;

	while (*from) {
		for (next = from; *next && *next != '/'; ++next);

		len = next - from;

		if (len == 1 && from[0] == '.')
			/* do nothing with singleton dot */;

		else if (len == 2 && from[0] == '.' && from[1] == '.') {
			/* error out if trying to up one from a hard base */
			if (to == base && ceiling != 0) {
				git_error_set(GIT_ERROR_INVALID,
					"cannot strip root component off url");
				return -1;
			}

			/* no more path segments to strip,
			 * use '../' as a new base path */
			if (to == base) {
				if (*next == '/')
					len++;

				if (to != from)
					memmove(to, from, len);

				to += len;
				/* this is now the base, can't back up from a
				 * relative prefix */
				base = to;
			} else {
				/* back up a path segment */
				while (to > base && to[-1] == '/') to--;
				while (to > base && to[-1] != '/') to--;
			}
		} else {
			if (*next == '/' && *from != '/')
				len++;

			if (to != from)
				memmove(to, from, len);

			to += len;
		}

		from += len;

		while (*from == '/') from++;
	}

	*to = '\0';

	path->size = to - path->ptr;

	return 0;
}