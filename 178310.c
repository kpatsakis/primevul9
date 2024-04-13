static gboolean try_remove_cache(gpointer user_data)
{
	cache_timer = 0;

	if (__sync_fetch_and_sub(&cache_refcount, 1) == 1) {
		debug("No cache users, removing it.");

		g_hash_table_destroy(cache);
		cache = NULL;
	}

	return FALSE;
}