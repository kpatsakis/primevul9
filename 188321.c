int git_path_diriter_next(git_path_diriter *diriter)
{
	bool skip_dot = !(diriter->flags & GIT_PATH_DIR_INCLUDE_DOT_AND_DOTDOT);

	do {
		/* Our first time through, we already have the data from
		 * FindFirstFileW.  Use it, otherwise get the next file.
		 */
		if (!diriter->needs_next)
			diriter->needs_next = 1;
		else if (!FindNextFileW(diriter->handle, &diriter->current))
			return GIT_ITEROVER;
	} while (skip_dot && git_path_is_dot_or_dotdotW(diriter->current.cFileName));

	if (diriter_update_paths(diriter) < 0)
		return -1;

	return 0;
}