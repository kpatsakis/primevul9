add_termcap_entry(char_u *name, int force)
{
    char_u  *term;
    int	    key;
    struct builtin_term *termp;
#ifdef HAVE_TGETENT
    char_u  *string;
    int	    i;
    int	    builtin_first;
    char_u  tbuf[TBUFSZ];
    char_u  tstrbuf[TBUFSZ];
    char_u  *tp = tstrbuf;
    char    *error_msg = NULL;
#endif

/*
 * If the GUI is running or will start in a moment, we only support the keys
 * that the GUI can produce.
 */
#ifdef FEAT_GUI
    if (gui.in_use || gui.starting)
	return gui_mch_haskey(name);
#endif

    if (!force && find_termcode(name) != NULL)	    // it's already there
	return OK;

    term = T_NAME;
    if (term == NULL || *term == NUL)	    // 'term' not defined yet
	return FAIL;

    if (term_is_builtin(term))		    // name starts with "builtin_"
    {
	term += 8;
#ifdef HAVE_TGETENT
	builtin_first = TRUE;
#endif
    }
#ifdef HAVE_TGETENT
    else
	builtin_first = p_tbi;
#endif

#ifdef HAVE_TGETENT
/*
 * We can get the entry from the builtin termcap and from the external one.
 * If 'ttybuiltin' is on or the terminal name starts with "builtin_", try
 * builtin termcap first.
 * If 'ttybuiltin' is off, try external termcap first.
 */
    for (i = 0; i < 2; ++i)
    {
	if ((!builtin_first) == i)
#endif
	/*
	 * Search in builtin termcap
	 */
	{
	    termp = find_builtin_term(term);
	    if (termp->bt_string != NULL)	// found it
	    {
		key = TERMCAP2KEY(name[0], name[1]);
		++termp;
		while (termp->bt_entry != (int)KS_NAME)
		{
		    if ((int)termp->bt_entry == key)
		    {
			add_termcode(name, (char_u *)termp->bt_string,
							  term_is_8bit(term));
			return OK;
		    }
		    ++termp;
		}
	    }
	}
#ifdef HAVE_TGETENT
	else
	/*
	 * Search in external termcap
	 */
	{
	    error_msg = invoke_tgetent(tbuf, term);
	    if (error_msg == NULL)
	    {
		string = TGETSTR((char *)name, &tp);
		if (string != NULL && *string != NUL)
		{
		    add_termcode(name, string, FALSE);
		    return OK;
		}
	    }
	}
    }
#endif

    if (SOURCING_NAME == NULL)
    {
#ifdef HAVE_TGETENT
	if (error_msg != NULL)
	    emsg(error_msg);
	else
#endif
	    semsg(_(e_no_str_entry_in_termcap), name);
    }
    return FAIL;
}