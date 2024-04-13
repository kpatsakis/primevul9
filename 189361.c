ExpandSettings(
    expand_T	*xp,
    regmatch_T	*regmatch,
    int		*num_file,
    char_u	***file)
{
    int		num_normal = 0;	    /* Nr of matching non-term-code settings */
    int		num_term = 0;	    /* Nr of matching terminal code settings */
    int		opt_idx;
    int		match;
    int		count = 0;
    char_u	*str;
    int		loop;
    int		is_term_opt;
    char_u	name_buf[MAX_KEY_NAME_LEN];
    static char *(names[]) = {"all", "termcap"};
    int		ic = regmatch->rm_ic;	/* remember the ignore-case flag */

    /* do this loop twice:
     * loop == 0: count the number of matching options
     * loop == 1: copy the matching options into allocated memory
     */
    for (loop = 0; loop <= 1; ++loop)
    {
	regmatch->rm_ic = ic;
	if (xp->xp_context != EXPAND_BOOL_SETTINGS)
	{
	    for (match = 0; match < (int)(sizeof(names) / sizeof(char *));
								      ++match)
		if (vim_regexec(regmatch, (char_u *)names[match], (colnr_T)0))
		{
		    if (loop == 0)
			num_normal++;
		    else
			(*file)[count++] = vim_strsave((char_u *)names[match]);
		}
	}
	for (opt_idx = 0; (str = (char_u *)options[opt_idx].fullname) != NULL;
								    opt_idx++)
	{
	    if (options[opt_idx].var == NULL)
		continue;
	    if (xp->xp_context == EXPAND_BOOL_SETTINGS
	      && !(options[opt_idx].flags & P_BOOL))
		continue;
	    is_term_opt = istermoption(&options[opt_idx]);
	    if (is_term_opt && num_normal > 0)
		continue;
	    match = FALSE;
	    if (vim_regexec(regmatch, str, (colnr_T)0)
		    || (options[opt_idx].shortname != NULL
			&& vim_regexec(regmatch,
			   (char_u *)options[opt_idx].shortname, (colnr_T)0)))
		match = TRUE;
	    else if (is_term_opt)
	    {
		name_buf[0] = '<';
		name_buf[1] = 't';
		name_buf[2] = '_';
		name_buf[3] = str[2];
		name_buf[4] = str[3];
		name_buf[5] = '>';
		name_buf[6] = NUL;
		if (vim_regexec(regmatch, name_buf, (colnr_T)0))
		{
		    match = TRUE;
		    str = name_buf;
		}
	    }
	    if (match)
	    {
		if (loop == 0)
		{
		    if (is_term_opt)
			num_term++;
		    else
			num_normal++;
		}
		else
		    (*file)[count++] = vim_strsave(str);
	    }
	}
	/*
	 * Check terminal key codes, these are not in the option table
	 */
	if (xp->xp_context != EXPAND_BOOL_SETTINGS  && num_normal == 0)
	{
	    for (opt_idx = 0; (str = get_termcode(opt_idx)) != NULL; opt_idx++)
	    {
		if (!isprint(str[0]) || !isprint(str[1]))
		    continue;

		name_buf[0] = 't';
		name_buf[1] = '_';
		name_buf[2] = str[0];
		name_buf[3] = str[1];
		name_buf[4] = NUL;

		match = FALSE;
		if (vim_regexec(regmatch, name_buf, (colnr_T)0))
		    match = TRUE;
		else
		{
		    name_buf[0] = '<';
		    name_buf[1] = 't';
		    name_buf[2] = '_';
		    name_buf[3] = str[0];
		    name_buf[4] = str[1];
		    name_buf[5] = '>';
		    name_buf[6] = NUL;

		    if (vim_regexec(regmatch, name_buf, (colnr_T)0))
			match = TRUE;
		}
		if (match)
		{
		    if (loop == 0)
			num_term++;
		    else
			(*file)[count++] = vim_strsave(name_buf);
		}
	    }

	    /*
	     * Check special key names.
	     */
	    regmatch->rm_ic = TRUE;		/* ignore case here */
	    for (opt_idx = 0; (str = get_key_name(opt_idx)) != NULL; opt_idx++)
	    {
		name_buf[0] = '<';
		STRCPY(name_buf + 1, str);
		STRCAT(name_buf, ">");

		if (vim_regexec(regmatch, name_buf, (colnr_T)0))
		{
		    if (loop == 0)
			num_term++;
		    else
			(*file)[count++] = vim_strsave(name_buf);
		}
	    }
	}
	if (loop == 0)
	{
	    if (num_normal > 0)
		*num_file = num_normal;
	    else if (num_term > 0)
		*num_file = num_term;
	    else
		return OK;
	    *file = (char_u **)alloc((unsigned)(*num_file * sizeof(char_u *)));
	    if (*file == NULL)
	    {
		*file = (char_u **)"";
		return FAIL;
	    }
	}
    }
    return OK;
}