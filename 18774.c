clear_termcodes(void)
{
    while (tc_len > 0)
	vim_free(termcodes[--tc_len].code);
    VIM_CLEAR(termcodes);
    tc_max_len = 0;

#ifdef HAVE_TGETENT
    BC = (char *)empty_option;
    UP = (char *)empty_option;
    PC = NUL;			// set pad character to NUL
    ospeed = 0;
#endif

    need_gather = TRUE;		// need to fill termleader[]
}