const char *setup_git_directory(void)
{
	const char *retval = setup_git_directory_gently(NULL);

	/* If the work tree is not the default one, recompute prefix */
	if (inside_work_tree < 0) {
		static char buffer[PATH_MAX + 1];
		char *rel;
		if (retval && chdir(retval))
			die_errno ("Could not jump back into original cwd");
		rel = get_relative_cwd(buffer, PATH_MAX, get_git_work_tree());
		if (rel && *rel && chdir(get_git_work_tree()))
			die_errno ("Could not jump to working directory");
		return rel && *rel ? strcat(rel, "/") : NULL;
	}

	return retval;
}