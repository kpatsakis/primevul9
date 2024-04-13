mono_double_ParseImpl (char *ptr, double *result)
{
	gchar *endptr = NULL;
	*result = 0.0;

	MONO_ARCH_SAVE_REGS;

#ifdef __arm__
	if (*ptr)
		*result = strtod (ptr, &endptr);
#else
	if (*ptr){
#ifdef _EGLIB_MAJOR
		/* Need to lock here because EGLIB (#464316) has locking defined as no-ops, and that breaks mono_strtod */
		EnterCriticalSection (&mono_strtod_mutex);
		*result = mono_strtod (ptr, &endptr);
		LeaveCriticalSection (&mono_strtod_mutex);
#else
		*result = mono_strtod (ptr, &endptr);
#endif
	}
#endif

	if (!*ptr || (endptr && *endptr))
		return FALSE;
	
	return TRUE;
}