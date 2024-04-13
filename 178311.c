static void cache_element_destroy(gpointer value)
{
	struct cache_entry *entry = value;

	if (!entry)
		return;

	if (entry->ipv4) {
		g_free(entry->ipv4->data);
		g_free(entry->ipv4);
	}

	if (entry->ipv6) {
		g_free(entry->ipv6->data);
		g_free(entry->ipv6);
	}

	g_free(entry->key);
	g_free(entry);

	if (--cache_size < 0)
		cache_size = 0;
}