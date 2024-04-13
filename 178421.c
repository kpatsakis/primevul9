regtilde(char_u *source, int magic)
{
    char_u	*newsub = source;
    char_u	*tmpsub;
    char_u	*p;
    int		len;
    int		prevlen;

    for (p = newsub; *p; ++p)
    {
	if ((*p == '~' && magic) || (*p == '\\' && *(p + 1) == '~' && !magic))
	{
	    if (reg_prev_sub != NULL)
	    {
		// length = len(newsub) - 1 + len(prev_sub) + 1
		prevlen = (int)STRLEN(reg_prev_sub);
		tmpsub = alloc(STRLEN(newsub) + prevlen);
		if (tmpsub != NULL)
		{
		    // copy prefix
		    len = (int)(p - newsub);	// not including ~
		    mch_memmove(tmpsub, newsub, (size_t)len);
		    // interpret tilde
		    mch_memmove(tmpsub + len, reg_prev_sub, (size_t)prevlen);
		    // copy postfix
		    if (!magic)
			++p;			// back off backslash
		    STRCPY(tmpsub + len + prevlen, p + 1);

		    if (newsub != source)	// already allocated newsub
			vim_free(newsub);
		    newsub = tmpsub;
		    p = newsub + len + prevlen;
		}
	    }
	    else if (magic)
		STRMOVE(p, p + 1);	// remove '~'
	    else
		STRMOVE(p, p + 2);	// remove '\~'
	    --p;
	}
	else
	{
	    if (*p == '\\' && p[1])		// skip escaped characters
		++p;
	    if (has_mbyte)
		p += (*mb_ptr2len)(p) - 1;
	}
    }

    // Store a copy of newsub  in reg_prev_sub.  It is always allocated,
    // because recursive calls may make the returned string invalid.
    vim_free(reg_prev_sub);
    reg_prev_sub = vim_strsave(newsub);

    return newsub;
}