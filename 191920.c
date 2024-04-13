NUM_cache_remove(NUMCacheEntry *ent)
{
#ifdef DEBUG_TO_FROM_CHAR
	elog(DEBUG_elog_output, "REMOVING ENTRY (%s)", ent->str);
#endif
	ent->str[0] = '\0';
	ent->age = 0;
}