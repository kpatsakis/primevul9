NUM_cache_search(char *str)
{
	int			i;
	NUMCacheEntry *ent;

	/* counter overflow check - paranoia? */
	if (NUMCounter >= (INT_MAX - NUM_CACHE_FIELDS - 1))
	{
		NUMCounter = 0;

		for (ent = NUMCache; ent <= (NUMCache + NUM_CACHE_FIELDS); ent++)
			ent->age = (++NUMCounter);
	}

	for (i = 0, ent = NUMCache; i < n_NUMCache; i++, ent++)
	{
		if (strcmp(ent->str, str) == 0)
		{
			ent->age = (++NUMCounter);
			last_NUMCacheEntry = ent;
			return ent;
		}
	}

	return NULL;
}