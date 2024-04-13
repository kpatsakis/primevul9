static void cache_cleanup(void)
{
	static int max_timeout;
	struct cache_timeout data;
	int count = 0;

	data.current_time = time(NULL);
	data.max_timeout = 0;
	data.try_harder = 0;

	/*
	 * In the first pass, we only remove entries that have timed out.
	 * We use a cache of the first time to expire to do this only
	 * when it makes sense.
	 */
	if (max_timeout <= data.current_time) {
		count = g_hash_table_foreach_remove(cache, cache_check_entry,
						&data);
	}
	debug("removed %d in the first pass", count);

	/*
	 * In the second pass, if the first pass turned up blank,
	 * we also expire entries with a low hit count,
	 * while aging the hit count at the same time.
	 */
	data.try_harder = 1;
	if (count == 0)
		count = g_hash_table_foreach_remove(cache, cache_check_entry,
						&data);

	if (count == 0)
		/*
		 * If we could not remove anything, then remember
		 * what is the max timeout and do nothing if we
		 * have not yet reached it. This will prevent
		 * constant traversal of the cache if it is full.
		 */
		max_timeout = data.max_timeout;
	else
		max_timeout = 0;
}