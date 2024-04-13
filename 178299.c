static void cache_refresh_iterator(gpointer key, gpointer value,
					gpointer user_data)
{
	struct cache_entry *entry = value;

	cache_refresh_entry(entry);
}