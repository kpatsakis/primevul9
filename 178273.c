static void cache_invalidate(void)
{
	debug("Invalidating the DNS cache %p", cache);

	if (!cache)
		return;

	g_hash_table_foreach_remove(cache, cache_invalidate_entry, NULL);
}