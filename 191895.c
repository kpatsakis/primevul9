DCH_cache_search(char *str)
{
	int			i;
	DCHCacheEntry *ent;

	/* counter overflow check - paranoia? */
	if (DCHCounter >= (INT_MAX - DCH_CACHE_FIELDS - 1))
	{
		DCHCounter = 0;

		for (ent = DCHCache; ent <= (DCHCache + DCH_CACHE_FIELDS); ent++)
			ent->age = (++DCHCounter);
	}

	for (i = 0, ent = DCHCache; i < n_DCHCache; i++, ent++)
	{
		if (strcmp(ent->str, str) == 0)
		{
			ent->age = (++DCHCounter);
			return ent;
		}
	}

	return NULL;
}