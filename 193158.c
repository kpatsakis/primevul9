int parse_submodule_config_option(const char *var, const char *value)
{
	return submodule_config_option(the_repository, var, value);
}