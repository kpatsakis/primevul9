seq_search(char *name, const char *const * array, int type, int max, int *len)
{
	const char *p;
	const char *const * a;
	char	   *n;
	int			last,
				i;

	*len = 0;

	if (!*name)
		return -1;

	/* set first char */
	if (type == ONE_UPPER || type == ALL_UPPER)
		*name = pg_toupper((unsigned char) *name);
	else if (type == ALL_LOWER)
		*name = pg_tolower((unsigned char) *name);

	for (last = 0, a = array; *a != NULL; a++)
	{
		/* comperate first chars */
		if (*name != **a)
			continue;

		for (i = 1, p = *a + 1, n = name + 1;; n++, p++, i++)
		{
			/* search fragment (max) only */
			if (max && i == max)
			{
				*len = i;
				return a - array;
			}
			/* full size */
			if (*p == '\0')
			{
				*len = i;
				return a - array;
			}
			/* Not found in array 'a' */
			if (*n == '\0')
				break;

			/*
			 * Convert (but convert new chars only)
			 */
			if (i > last)
			{
				if (type == ONE_UPPER || type == ALL_LOWER)
					*n = pg_tolower((unsigned char) *n);
				else if (type == ALL_UPPER)
					*n = pg_toupper((unsigned char) *n);
				last = i;
			}

#ifdef DEBUG_TO_FROM_CHAR
			elog(DEBUG_elog_output, "N: %c, P: %c, A: %s (%s)",
				 *n, *p, *a, name);
#endif
			if (*n != *p)
				break;
		}
	}

	return -1;
}