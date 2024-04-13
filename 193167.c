const struct submodule *submodule_from_cache(struct repository *repo,
					     const unsigned char *treeish_name,
					     const char *key)
{
	submodule_cache_check_init(repo);
	return config_from(repo->submodule_cache, treeish_name,
			   key, lookup_path);
}