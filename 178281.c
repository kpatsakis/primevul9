static gboolean cache_invalidate_entry(gpointer key, gpointer value,
					gpointer user_data)
{
	struct cache_entry *entry = value;

	/* first, delete any expired elements */
	cache_enforce_validity(entry);

	/* if anything is not expired, mark the entry for refresh */
	if (entry->hits > 0 && (entry->ipv4 || entry->ipv6))
		entry->want_refresh = true;

	/* delete the cached data */
	if (entry->ipv4) {
		g_free(entry->ipv4->data);
		g_free(entry->ipv4);
		entry->ipv4 = NULL;
	}

	if (entry->ipv6) {
		g_free(entry->ipv6->data);
		g_free(entry->ipv6);
		entry->ipv6 = NULL;
	}

	/* keep the entry if we want it refreshed, delete it otherwise */
	if (entry->want_refresh)
		return FALSE;
	else
		return TRUE;
}