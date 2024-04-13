static void cache_enforce_validity(struct cache_entry *entry)
{
	time_t current_time = time(NULL);

	if (!cache_check_is_valid(entry->ipv4, current_time)
							&& entry->ipv4) {
		debug("cache timeout \"%s\" type A", entry->key);
		g_free(entry->ipv4->data);
		g_free(entry->ipv4);
		entry->ipv4 = NULL;

	}

	if (!cache_check_is_valid(entry->ipv6, current_time)
							&& entry->ipv6) {
		debug("cache timeout \"%s\" type AAAA", entry->key);
		g_free(entry->ipv6->data);
		g_free(entry->ipv6);
		entry->ipv6 = NULL;
	}
}