gather_termleader(void)
{
    int	    i;
    int	    len = 0;

#ifdef FEAT_GUI
    if (gui.in_use)
	termleader[len++] = CSI;    // the GUI codes are not in termcodes[]
#endif
#ifdef FEAT_TERMRESPONSE
    if (check_for_codes || *T_CRS != NUL)
	termleader[len++] = DCS;    // the termcode response starts with DCS
				    // in 8-bit mode
#endif
    termleader[len] = NUL;

    for (i = 0; i < tc_len; ++i)
	if (vim_strchr(termleader, termcodes[i].code[0]) == NULL)
	{
	    termleader[len++] = termcodes[i].code[0];
	    termleader[len] = NUL;
	}

    need_gather = FALSE;
}