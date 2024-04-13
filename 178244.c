static void cache_refresh(void)
{
	if (!cache)
		return;

	g_hash_table_foreach(cache, cache_refresh_iterator, NULL);
}