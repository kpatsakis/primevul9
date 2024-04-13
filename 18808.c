out_str_cf(char_u *s)
{
    if (s != NULL && *s)
    {
#ifdef HAVE_TGETENT
	char_u *p;
#endif

#ifdef FEAT_GUI
	// Don't use tputs() when GUI is used, ncurses crashes.
	if (gui.in_use)
	{
	    out_str_nf(s);
	    return;
	}
#endif
	if (out_pos > OUT_SIZE - MAX_ESC_SEQ_LEN)
	    out_flush();
#ifdef HAVE_TGETENT
	for (p = s; *s; ++s)
	{
	    // flush just before delay command
	    if (*s == '$' && *(s + 1) == '<')
	    {
		char_u save_c = *s;
		int duration = atoi((char *)s + 2);

		*s = NUL;
		tputs((char *)p, 1, TPUTSFUNCAST out_char_nf);
		*s = save_c;
		out_flush();
# ifdef ELAPSED_FUNC
		// Only sleep here if we can limit this happening in
		// vim_beep().
		p = vim_strchr(s, '>');
		if (p == NULL || duration <= 0)
		{
		    // can't parse the time, don't sleep here
		    p = s;
		}
		else
		{
		    ++p;
		    do_sleep(duration, FALSE);
		}
# else
		// Rely on the terminal library to sleep.
		p = s;
# endif
		break;
	    }
	}
	tputs((char *)p, 1, TPUTSFUNCAST out_char_nf);
#else
	while (*s)
	    out_char_nf(*s++);
#endif

	// For testing we write one string at a time.
	if (p_wd)
	    out_flush();
    }
}