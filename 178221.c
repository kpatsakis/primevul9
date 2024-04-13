int check_repository_format_version(const char *var, const char *value, void *cb)
{
	if (strcmp(var, "core.repositoryformatversion") == 0)
		repository_format_version = git_config_int(var, value);
	else if (strcmp(var, "core.sharedrepository") == 0)
		shared_repository = git_config_perm(var, value);
	else if (strcmp(var, "core.bare") == 0) {
		is_bare_repository_cfg = git_config_bool(var, value);
		if (is_bare_repository_cfg == 1)
			inside_work_tree = -1;
	} else if (strcmp(var, "core.worktree") == 0) {
		if (!value)
			return config_error_nonbool(var);
		free(git_work_tree_cfg);
		git_work_tree_cfg = xstrdup(value);
		inside_work_tree = -1;
	}
	return 0;
}