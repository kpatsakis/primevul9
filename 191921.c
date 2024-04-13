DCH_cache_getnew(char *str)
{
	DCHCacheEntry *ent;

	/* counter overflow check - paranoia? */
	if (DCHCounter >= (INT_MAX - DCH_CACHE_FIELDS - 1))
	{
		DCHCounter = 0;

		for (ent = DCHCache; ent <= (DCHCache + DCH_CACHE_FIELDS); ent++)
			ent->age = (++DCHCounter);
	}

	/*
	 * If cache is full, remove oldest entry
	 */
	if (n_DCHCache > DCH_CACHE_FIELDS)
	{
		DCHCacheEntry *old = DCHCache + 0;

#ifdef DEBUG_TO_FROM_CHAR
		elog(DEBUG_elog_output, "cache is full (%d)", n_DCHCache);
#endif
		for (ent = DCHCache + 1; ent <= (DCHCache + DCH_CACHE_FIELDS); ent++)
		{
			if (ent->age < old->age)
				old = ent;
		}
#ifdef DEBUG_TO_FROM_CHAR
		elog(DEBUG_elog_output, "OLD: '%s' AGE: %d", old->str, old->age);
#endif
		StrNCpy(old->str, str, DCH_CACHE_SIZE + 1);
		/* old->format fill parser */
		old->age = (++DCHCounter);
		return old;
	}
	else
	{
#ifdef DEBUG_TO_FROM_CHAR
		elog(DEBUG_elog_output, "NEW (%d)", n_DCHCache);
#endif
		ent = DCHCache + n_DCHCache;
		StrNCpy(ent->str, str, DCH_CACHE_SIZE + 1);
		/* ent->format fill parser */
		ent->age = (++DCHCounter);
		++n_DCHCache;
		return ent;
	}
}