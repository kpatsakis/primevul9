out_str(char_u *s)
{
    if (s != NULL && *s)
    {
#ifdef FEAT_GUI
	// Don't use tputs() when GUI is used, ncurses crashes.
	if (gui.in_use)
	{
	    out_str_nf(s);
	    return;
	}
#endif
	// avoid terminal strings being split up
	if (out_pos > OUT_SIZE - MAX_ESC_SEQ_LEN)
	    out_flush();
#ifdef HAVE_TGETENT
	tputs((char *)s, 1, TPUTSFUNCAST out_char_nf);
#else
	while (*s)
	    out_char_nf(*s++);
#endif

	// For testing we write one string at a time.
	if (p_wd)
	    out_flush();
    }
}