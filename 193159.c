static int config_path_cmp(const void *unused_cmp_data,
			   const struct submodule_entry *a,
			   const struct submodule_entry *b,
			   const void *unused_keydata)
{
	return strcmp(a->config->path, b->config->path) ||
	       hashcmp(a->config->gitmodules_sha1, b->config->gitmodules_sha1);
}