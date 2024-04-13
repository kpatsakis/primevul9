const struct submodule *submodule_from_path(const unsigned char *treeish_name,
		const char *path)
{
	submodule_cache_check_init(the_repository);
	return config_from(the_repository->submodule_cache, treeish_name, path, lookup_path);
}