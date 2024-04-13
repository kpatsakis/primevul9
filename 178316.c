static void create_cache(void)
{
	if (__sync_fetch_and_add(&cache_refcount, 1) == 0)
		cache = g_hash_table_new_full(g_str_hash,
					g_str_equal,
					NULL,
					cache_element_destroy);
}