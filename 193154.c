int submodule_config_option(struct repository *repo,
			    const char *var, const char *value)
{
	struct parse_config_parameter parameter;

	submodule_cache_check_init(repo);

	parameter.cache = repo->submodule_cache;
	parameter.treeish_name = NULL;
	parameter.gitmodules_sha1 = null_sha1;
	parameter.overwrite = 1;

	return parse_config(var, value, &parameter);
}