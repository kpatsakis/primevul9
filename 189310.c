findoption(char_u *arg)
{
    int		    opt_idx;
    char	    *s, *p;
    static short    quick_tab[27] = {0, 0};	/* quick access table */
    int		    is_term_opt;

    /*
     * For first call: Initialize the quick-access table.
     * It contains the index for the first option that starts with a certain
     * letter.  There are 26 letters, plus the first "t_" option.
     */
    if (quick_tab[1] == 0)
    {
	p = options[0].fullname;
	for (opt_idx = 1; (s = options[opt_idx].fullname) != NULL; opt_idx++)
	{
	    if (s[0] != p[0])
	    {
		if (s[0] == 't' && s[1] == '_')
		    quick_tab[26] = opt_idx;
		else
		    quick_tab[CharOrdLow(s[0])] = opt_idx;
	    }
	    p = s;
	}
    }

    /*
     * Check for name starting with an illegal character.
     */
#ifdef EBCDIC
    if (!islower(arg[0]))
#else
    if (arg[0] < 'a' || arg[0] > 'z')
#endif
	return -1;

    is_term_opt = (arg[0] == 't' && arg[1] == '_');
    if (is_term_opt)
	opt_idx = quick_tab[26];
    else
	opt_idx = quick_tab[CharOrdLow(arg[0])];
    for ( ; (s = options[opt_idx].fullname) != NULL; opt_idx++)
    {
	if (STRCMP(arg, s) == 0)		    /* match full name */
	    break;
    }
    if (s == NULL && !is_term_opt)
    {
	opt_idx = quick_tab[CharOrdLow(arg[0])];
	for ( ; options[opt_idx].fullname != NULL; opt_idx++)
	{
	    s = options[opt_idx].shortname;
	    if (s != NULL && STRCMP(arg, s) == 0)   /* match short name */
		break;
	    s = NULL;
	}
    }
    if (s == NULL)
	opt_idx = -1;
    return opt_idx;
}