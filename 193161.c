static const struct submodule *config_from(struct submodule_cache *cache,
		const unsigned char *treeish_name, const char *key,
		enum lookup_type lookup_type)
{
	struct strbuf rev = STRBUF_INIT;
	unsigned long config_size;
	char *config = NULL;
	unsigned char sha1[20];
	enum object_type type;
	const struct submodule *submodule = NULL;
	struct parse_config_parameter parameter;

	/*
	 * If any parameter except the cache is a NULL pointer just
	 * return the first submodule. Can be used to check whether
	 * there are any submodules parsed.
	 */
	if (!treeish_name || !key) {
		struct hashmap_iter iter;
		struct submodule_entry *entry;

		entry = hashmap_iter_first(&cache->for_name, &iter);
		if (!entry)
			return NULL;
		return entry->config;
	}

	if (!gitmodule_sha1_from_commit(treeish_name, sha1, &rev))
		goto out;

	switch (lookup_type) {
	case lookup_name:
		submodule = cache_lookup_name(cache, sha1, key);
		break;
	case lookup_path:
		submodule = cache_lookup_path(cache, sha1, key);
		break;
	}
	if (submodule)
		goto out;

	config = read_sha1_file(sha1, &type, &config_size);
	if (!config || type != OBJ_BLOB)
		goto out;

	/* fill the submodule config into the cache */
	parameter.cache = cache;
	parameter.treeish_name = treeish_name;
	parameter.gitmodules_sha1 = sha1;
	parameter.overwrite = 0;
	git_config_from_mem(parse_config, CONFIG_ORIGIN_SUBMODULE_BLOB, rev.buf,
			config, config_size, &parameter);
	strbuf_release(&rev);
	free(config);

	switch (lookup_type) {
	case lookup_name:
		return cache_lookup_name(cache, sha1, key);
	case lookup_path:
		return cache_lookup_path(cache, sha1, key);
	default:
		return NULL;
	}

out:
	strbuf_release(&rev);
	free(config);
	return submodule;
}