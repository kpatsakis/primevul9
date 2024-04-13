const char *setup_git_directory_gently(int *nongit_ok)
{
	const char *work_tree_env = getenv(GIT_WORK_TREE_ENVIRONMENT);
	const char *env_ceiling_dirs = getenv(CEILING_DIRECTORIES_ENVIRONMENT);
	static char cwd[PATH_MAX+1];
	const char *gitdirenv;
	const char *gitfile_dir;
	int len, offset, ceil_offset, root_len;

	/*
	 * Let's assume that we are in a git repository.
	 * If it turns out later that we are somewhere else, the value will be
	 * updated accordingly.
	 */
	if (nongit_ok)
		*nongit_ok = 0;

	/*
	 * If GIT_DIR is set explicitly, we're not going
	 * to do any discovery, but we still do repository
	 * validation.
	 */
	gitdirenv = getenv(GIT_DIR_ENVIRONMENT);
	if (gitdirenv) {
		if (PATH_MAX - 40 < strlen(gitdirenv))
			die("'$%s' too big", GIT_DIR_ENVIRONMENT);
		if (is_git_directory(gitdirenv)) {
			static char buffer[1024 + 1];
			const char *retval;

			if (!work_tree_env) {
				retval = set_work_tree(gitdirenv);
				/* config may override worktree */
				if (check_repository_format_gently(nongit_ok))
					return NULL;
				return retval;
			}
			if (check_repository_format_gently(nongit_ok))
				return NULL;
			retval = get_relative_cwd(buffer, sizeof(buffer) - 1,
					get_git_work_tree());
			if (!retval || !*retval)
				return NULL;
			set_git_dir(make_absolute_path(gitdirenv));
			if (chdir(work_tree_env) < 0)
				die_errno ("Could not chdir to '%s'", work_tree_env);
			strcat(buffer, "/");
			return retval;
		}
		if (nongit_ok) {
			*nongit_ok = 1;
			return NULL;
		}
		die("Not a git repository: '%s'", gitdirenv);
	}

	if (!getcwd(cwd, sizeof(cwd)-1))
		die_errno("Unable to read current working directory");

	ceil_offset = longest_ancestor_length(cwd, env_ceiling_dirs);
	if (ceil_offset < 0 && has_dos_drive_prefix(cwd))
		ceil_offset = 1;

	/*
	 * Test in the following order (relative to the cwd):
	 * - .git (file containing "gitdir: <path>")
	 * - .git/
	 * - ./ (bare)
	 * - ../.git
	 * - ../.git/
	 * - ../ (bare)
	 * - ../../.git/
	 *   etc.
	 */
	offset = len = strlen(cwd);
	for (;;) {
		gitfile_dir = read_gitfile_gently(DEFAULT_GIT_DIR_ENVIRONMENT);
		if (gitfile_dir) {
			if (set_git_dir(gitfile_dir))
				die("Repository setup failed");
			break;
		}
		if (is_git_directory(DEFAULT_GIT_DIR_ENVIRONMENT))
			break;
		if (is_git_directory(".")) {
			inside_git_dir = 1;
			if (!work_tree_env)
				inside_work_tree = 0;
			if (offset != len) {
				root_len = offset_1st_component(cwd);
				cwd[offset > root_len ? offset : root_len] = '\0';
				set_git_dir(cwd);
			} else
				set_git_dir(".");
			check_repository_format_gently(nongit_ok);
			return NULL;
		}
		while (--offset > ceil_offset && cwd[offset] != '/');
		if (offset <= ceil_offset) {
			if (nongit_ok) {
				if (chdir(cwd))
					die_errno("Cannot come back to cwd");
				*nongit_ok = 1;
				return NULL;
			}
			die("Not a git repository (or any of the parent directories): %s", DEFAULT_GIT_DIR_ENVIRONMENT);
		}
		if (chdir(".."))
			die_errno("Cannot change to '%s/..'", cwd);
	}

	inside_git_dir = 0;
	if (!work_tree_env)
		inside_work_tree = 1;
	root_len = offset_1st_component(cwd);
	git_work_tree_cfg = xstrndup(cwd, offset > root_len ? offset : root_len);
	if (check_repository_format_gently(nongit_ok))
		return NULL;
	if (offset == len)
		return NULL;

	/* Make "offset" point to past the '/', and add a '/' at the end */
	offset++;
	cwd[len++] = '/';
	cwd[len] = 0;
	return cwd + offset;
}