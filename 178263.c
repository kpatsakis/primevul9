static gboolean cache_check_entry(gpointer key, gpointer value,
					gpointer user_data)
{
	struct cache_timeout *data = user_data;
	struct cache_entry *entry = value;
	int max_timeout;

	/* Scale the number of hits by half as part of cache aging */

	entry->hits /= 2;

	/*
	 * If either IPv4 or IPv6 cached entry has expired, we
	 * remove both from the cache.
	 */

	if (entry->ipv4 && entry->ipv4->timeout > 0) {
		max_timeout = entry->ipv4->cache_until;
		if (max_timeout > data->max_timeout)
			data->max_timeout = max_timeout;

		if (entry->ipv4->cache_until < data->current_time)
			return TRUE;
	}

	if (entry->ipv6 && entry->ipv6->timeout > 0) {
		max_timeout = entry->ipv6->cache_until;
		if (max_timeout > data->max_timeout)
			data->max_timeout = max_timeout;

		if (entry->ipv6->cache_until < data->current_time)
			return TRUE;
	}

	/*
	 * if we're asked to try harder, also remove entries that have
	 * few hits
	 */
	if (data->try_harder && entry->hits < 4)
		return TRUE;

	return FALSE;
}