set_context_in_set_cmd(
    expand_T	*xp,
    char_u	*arg,
    int		opt_flags)	/* OPT_GLOBAL and/or OPT_LOCAL */
{
    int		nextchar;
    long_u	flags = 0;	/* init for GCC */
    int		opt_idx = 0;	/* init for GCC */
    char_u	*p;
    char_u	*s;
    int		is_term_option = FALSE;
    int		key;

    expand_option_flags = opt_flags;

    xp->xp_context = EXPAND_SETTINGS;
    if (*arg == NUL)
    {
	xp->xp_pattern = arg;
	return;
    }
    p = arg + STRLEN(arg) - 1;
    if (*p == ' ' && *(p - 1) != '\\')
    {
	xp->xp_pattern = p + 1;
	return;
    }
    while (p > arg)
    {
	s = p;
	/* count number of backslashes before ' ' or ',' */
	if (*p == ' ' || *p == ',')
	{
	    while (s > arg && *(s - 1) == '\\')
		--s;
	}
	/* break at a space with an even number of backslashes */
	if (*p == ' ' && ((p - s) & 1) == 0)
	{
	    ++p;
	    break;
	}
	--p;
    }
    if (STRNCMP(p, "no", 2) == 0 && STRNCMP(p, "novice", 6) != 0)
    {
	xp->xp_context = EXPAND_BOOL_SETTINGS;
	p += 2;
    }
    if (STRNCMP(p, "inv", 3) == 0)
    {
	xp->xp_context = EXPAND_BOOL_SETTINGS;
	p += 3;
    }
    xp->xp_pattern = arg = p;
    if (*arg == '<')
    {
	while (*p != '>')
	    if (*p++ == NUL)	    /* expand terminal option name */
		return;
	key = get_special_key_code(arg + 1);
	if (key == 0)		    /* unknown name */
	{
	    xp->xp_context = EXPAND_NOTHING;
	    return;
	}
	nextchar = *++p;
	is_term_option = TRUE;
	expand_option_name[2] = KEY2TERMCAP0(key);
	expand_option_name[3] = KEY2TERMCAP1(key);
    }
    else
    {
	if (p[0] == 't' && p[1] == '_')
	{
	    p += 2;
	    if (*p != NUL)
		++p;
	    if (*p == NUL)
		return;		/* expand option name */
	    nextchar = *++p;
	    is_term_option = TRUE;
	    expand_option_name[2] = p[-2];
	    expand_option_name[3] = p[-1];
	}
	else
	{
		/* Allow * wildcard */
	    while (ASCII_ISALNUM(*p) || *p == '_' || *p == '*')
		p++;
	    if (*p == NUL)
		return;
	    nextchar = *p;
	    *p = NUL;
	    opt_idx = findoption(arg);
	    *p = nextchar;
	    if (opt_idx == -1 || options[opt_idx].var == NULL)
	    {
		xp->xp_context = EXPAND_NOTHING;
		return;
	    }
	    flags = options[opt_idx].flags;
	    if (flags & P_BOOL)
	    {
		xp->xp_context = EXPAND_NOTHING;
		return;
	    }
	}
    }
    /* handle "-=" and "+=" */
    if ((nextchar == '-' || nextchar == '+' || nextchar == '^') && p[1] == '=')
    {
	++p;
	nextchar = '=';
    }
    if ((nextchar != '=' && nextchar != ':')
				    || xp->xp_context == EXPAND_BOOL_SETTINGS)
    {
	xp->xp_context = EXPAND_UNSUCCESSFUL;
	return;
    }
    if (xp->xp_context != EXPAND_BOOL_SETTINGS && p[1] == NUL)
    {
	xp->xp_context = EXPAND_OLD_SETTING;
	if (is_term_option)
	    expand_option_idx = -1;
	else
	    expand_option_idx = opt_idx;
	xp->xp_pattern = p + 1;
	return;
    }
    xp->xp_context = EXPAND_NOTHING;
    if (is_term_option || (flags & P_NUM))
	return;

    xp->xp_pattern = p + 1;

    if (flags & P_EXPAND)
    {
	p = options[opt_idx].var;
	if (p == (char_u *)&p_bdir
		|| p == (char_u *)&p_dir
		|| p == (char_u *)&p_path
		|| p == (char_u *)&p_pp
		|| p == (char_u *)&p_rtp
#ifdef FEAT_SEARCHPATH
		|| p == (char_u *)&p_cdpath
#endif
#ifdef FEAT_SESSION
		|| p == (char_u *)&p_vdir
#endif
		)
	{
	    xp->xp_context = EXPAND_DIRECTORIES;
	    if (p == (char_u *)&p_path
#ifdef FEAT_SEARCHPATH
		    || p == (char_u *)&p_cdpath
#endif
		   )
		xp->xp_backslash = XP_BS_THREE;
	    else
		xp->xp_backslash = XP_BS_ONE;
	}
	else
	{
	    xp->xp_context = EXPAND_FILES;
	    /* for 'tags' need three backslashes for a space */
	    if (p == (char_u *)&p_tags)
		xp->xp_backslash = XP_BS_THREE;
	    else
		xp->xp_backslash = XP_BS_ONE;
	}
    }

    /* For an option that is a list of file names, find the start of the
     * last file name. */
    for (p = arg + STRLEN(arg) - 1; p > xp->xp_pattern; --p)
    {
	/* count number of backslashes before ' ' or ',' */
	if (*p == ' ' || *p == ',')
	{
	    s = p;
	    while (s > xp->xp_pattern && *(s - 1) == '\\')
		--s;
	    if ((*p == ' ' && (xp->xp_backslash == XP_BS_THREE && (p - s) < 3))
		    || (*p == ',' && (flags & P_COMMA) && ((p - s) & 1) == 0))
	    {
		xp->xp_pattern = p + 1;
		break;
	    }
	}

#ifdef FEAT_SPELL
	/* for 'spellsuggest' start at "file:" */
	if (options[opt_idx].var == (char_u *)&p_sps
					       && STRNCMP(p, "file:", 5) == 0)
	{
	    xp->xp_pattern = p + 5;
	    break;
	}
#endif
    }

    return;
}