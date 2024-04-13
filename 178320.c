static uint16_t cache_check_validity(char *question, uint16_t type,
				struct cache_entry *entry)
{
	time_t current_time = time(NULL);
	bool want_refresh = false;

	/*
	 * if we have a popular entry, we want a refresh instead of
	 * total destruction of the entry.
	 */
	if (entry->hits > 2)
		want_refresh = true;

	cache_enforce_validity(entry);

	switch (type) {
	case 1:		/* IPv4 */
		if (!cache_check_is_valid(entry->ipv4, current_time)) {
			debug("cache %s \"%s\" type A", entry->ipv4 ?
					"timeout" : "entry missing", question);

			if (want_refresh)
				entry->want_refresh = true;

			/*
			 * We do not remove cache entry if there is still
			 * valid IPv6 entry found in the cache.
			 */
			if (!cache_check_is_valid(entry->ipv6, current_time) && !want_refresh) {
				g_hash_table_remove(cache, question);
				type = 0;
			}
		}
		break;

	case 28:	/* IPv6 */
		if (!cache_check_is_valid(entry->ipv6, current_time)) {
			debug("cache %s \"%s\" type AAAA", entry->ipv6 ?
					"timeout" : "entry missing", question);

			if (want_refresh)
				entry->want_refresh = true;

			if (!cache_check_is_valid(entry->ipv4, current_time) && !want_refresh) {
				g_hash_table_remove(cache, question);
				type = 0;
			}
		}
		break;
	}

	return type;
}