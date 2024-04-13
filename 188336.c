int git_path_find_dir(git_buf *dir, const char *path, const char *base)
{
	int error = git_path_join_unrooted(dir, path, base, NULL);

	if (!error) {
		char buf[GIT_PATH_MAX];
		if (p_realpath(dir->ptr, buf) != NULL)
			error = git_buf_sets(dir, buf);
	}

	/* call dirname if this is not a directory */
	if (!error) /* && git_path_isdir(dir->ptr) == false) */
		error = (git_path_dirname_r(dir, dir->ptr) < 0) ? -1 : 0;

	if (!error)
		error = git_path_to_dir(dir);

	return error;
}