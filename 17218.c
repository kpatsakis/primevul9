pathcmp(const char *p, const char *q, int maxlen)
{
    int		i, j;
    int		c1, c2;
    const char	*s = NULL;

    for (i = 0, j = 0; maxlen < 0 || (i < maxlen && j < maxlen);)
    {
	c1 = PTR2CHAR((char_u *)p + i);
	c2 = PTR2CHAR((char_u *)q + j);

	// End of "p": check if "q" also ends or just has a slash.
	if (c1 == NUL)
	{
	    if (c2 == NUL)  // full match
		return 0;
	    s = q;
	    i = j;
	    break;
	}

	// End of "q": check if "p" just has a slash.
	if (c2 == NUL)
	{
	    s = p;
	    break;
	}

	if ((p_fic ? MB_TOUPPER(c1) != MB_TOUPPER(c2) : c1 != c2)
#ifdef BACKSLASH_IN_FILENAME
		// consider '/' and '\\' to be equal
		&& !((c1 == '/' && c2 == '\\')
		    || (c1 == '\\' && c2 == '/'))
#endif
		)
	{
	    if (vim_ispathsep(c1))
		return -1;
	    if (vim_ispathsep(c2))
		return 1;
	    return p_fic ? MB_TOUPPER(c1) - MB_TOUPPER(c2)
		    : c1 - c2;  // no match
	}

	i += mb_ptr2len((char_u *)p + i);
	j += mb_ptr2len((char_u *)q + j);
    }
    if (s == NULL)	// "i" or "j" ran into "maxlen"
	return 0;

    c1 = PTR2CHAR((char_u *)s + i);
    c2 = PTR2CHAR((char_u *)s + i + mb_ptr2len((char_u *)s + i));
    // ignore a trailing slash, but not "//" or ":/"
    if (c2 == NUL
	    && i > 0
	    && !after_pathsep((char_u *)s, (char_u *)s + i)
#ifdef BACKSLASH_IN_FILENAME
	    && (c1 == '/' || c1 == '\\')
#else
	    && c1 == '/'
#endif
       )
	return 0;   // match with trailing slash
    if (s == q)
	return -1;	    // no match
    return 1;
}