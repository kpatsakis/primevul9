shorten_dir_len(char_u *str, int trim_len)
{
    char_u	*tail, *s, *d;
    int		skip = FALSE;
    int		dirchunk_len = 0;

    tail = gettail(str);
    d = str;
    for (s = str; ; ++s)
    {
	if (s >= tail)		    // copy the whole tail
	{
	    *d++ = *s;
	    if (*s == NUL)
		break;
	}
	else if (vim_ispathsep(*s))	    // copy '/' and next char
	{
	    *d++ = *s;
	    skip = FALSE;
	    dirchunk_len = 0;
	}
	else if (!skip)
	{
	    *d++ = *s;			// copy next char
	    if (*s != '~' && *s != '.') // and leading "~" and "."
	    {
		++dirchunk_len; // only count word chars for the size

		// keep copying chars until we have our preferred length (or
		// until the above if/else branches move us along)
		if (dirchunk_len >= trim_len)
		    skip = TRUE;
	    }

	    if (has_mbyte)
	    {
		int l = mb_ptr2len(s);

		while (--l > 0)
		    *d++ = *++s;
	    }
	}
    }
}