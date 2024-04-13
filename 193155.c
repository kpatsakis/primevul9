static void submodule_cache_clear(struct submodule_cache *cache)
{
	struct hashmap_iter iter;
	struct submodule_entry *entry;

	if (!cache->initialized)
		return;

	/*
	 * We iterate over the name hash here to be symmetric with the
	 * allocation of struct submodule entries. Each is allocated by
	 * their .gitmodule blob sha1 and submodule name.
	 */
	hashmap_iter_init(&cache->for_name, &iter);
	while ((entry = hashmap_iter_next(&iter)))
		free_one_config(entry);

	hashmap_free(&cache->for_path, 1);
	hashmap_free(&cache->for_name, 1);
	cache->initialized = 0;
}