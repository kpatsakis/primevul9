skip_regexp_ex(
    char_u	*startp,
    int		dirc,
    int		magic,
    char_u	**newp,
    int		*dropped,
    magic_T	*magic_val)
{
    magic_T	mymagic;
    char_u	*p = startp;

    if (magic)
	mymagic = MAGIC_ON;
    else
	mymagic = MAGIC_OFF;
    get_cpo_flags();

    for (; p[0] != NUL; MB_PTR_ADV(p))
    {
	if (p[0] == dirc)	// found end of regexp
	    break;
	if ((p[0] == '[' && mymagic >= MAGIC_ON)
		|| (p[0] == '\\' && p[1] == '[' && mymagic <= MAGIC_OFF))
	{
	    p = skip_anyof(p + 1);
	    if (p[0] == NUL)
		break;
	}
	else if (p[0] == '\\' && p[1] != NUL)
	{
	    if (dirc == '?' && newp != NULL && p[1] == '?')
	    {
		// change "\?" to "?", make a copy first.
		if (*newp == NULL)
		{
		    *newp = vim_strsave(startp);
		    if (*newp != NULL)
			p = *newp + (p - startp);
		}
		if (dropped != NULL)
		    ++*dropped;
		if (*newp != NULL)
		    STRMOVE(p, p + 1);
		else
		    ++p;
	    }
	    else
		++p;    // skip next character
	    if (*p == 'v')
		mymagic = MAGIC_ALL;
	    else if (*p == 'V')
		mymagic = MAGIC_NONE;
	}
    }
    if (magic_val != NULL)
	*magic_val = mymagic;
    return p;
}