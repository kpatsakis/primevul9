NUM_cache_getnew(char *str)
{
	NUMCacheEntry *ent;

	/* counter overflow check - paranoia? */
	if (NUMCounter >= (INT_MAX - NUM_CACHE_FIELDS - 1))
	{
		NUMCounter = 0;

		for (ent = NUMCache; ent <= (NUMCache + NUM_CACHE_FIELDS); ent++)
			ent->age = (++NUMCounter);
	}

	/*
	 * If cache is full, remove oldest entry
	 */
	if (n_NUMCache > NUM_CACHE_FIELDS)
	{
		NUMCacheEntry *old = NUMCache + 0;

#ifdef DEBUG_TO_FROM_CHAR
		elog(DEBUG_elog_output, "Cache is full (%d)", n_NUMCache);
#endif
		for (ent = NUMCache; ent <= (NUMCache + NUM_CACHE_FIELDS); ent++)
		{
			/*
			 * entry removed via NUM_cache_remove() can be used here, which is
			 * why it's worth scanning first entry again
			 */
			if (ent->str[0] == '\0')
			{
				old = ent;
				break;
			}
			if (ent->age < old->age)
				old = ent;
		}
#ifdef DEBUG_TO_FROM_CHAR
		elog(DEBUG_elog_output, "OLD: \"%s\" AGE: %d", old->str, old->age);
#endif
		StrNCpy(old->str, str, NUM_CACHE_SIZE + 1);
		/* old->format fill parser */
		old->age = (++NUMCounter);
		ent = old;
	}
	else
	{
#ifdef DEBUG_TO_FROM_CHAR
		elog(DEBUG_elog_output, "NEW (%d)", n_NUMCache);
#endif
		ent = NUMCache + n_NUMCache;
		StrNCpy(ent->str, str, NUM_CACHE_SIZE + 1);
		/* ent->format fill parser */
		ent->age = (++NUMCounter);
		++n_NUMCache;
	}

	zeroize_NUM(&ent->Num);

	last_NUMCacheEntry = ent;
	return ent;
}