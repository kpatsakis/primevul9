const struct submodule *submodule_from_name(const unsigned char *treeish_name,
		const char *name)
{
	submodule_cache_check_init(the_repository);
	return config_from(the_repository->submodule_cache, treeish_name, name, lookup_name);
}