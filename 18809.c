invoke_tgetent(char_u *tbuf, char_u *term)
{
    int	    i;

    // Note: Valgrind may report a leak here, because the library keeps one
    // buffer around that we can't ever free.
    i = TGETENT(tbuf, term);
    if (i < 0		    // -1 is always an error
# ifdef TGETENT_ZERO_ERR
	    || i == 0	    // sometimes zero is also an error
# endif
       )
    {
	// On FreeBSD tputs() gets a SEGV after a tgetent() which fails.  Call
	// tgetent() with the always existing "dumb" entry to avoid a crash or
	// hang.
	(void)TGETENT(tbuf, "dumb");

	if (i < 0)
# ifdef TGETENT_ZERO_ERR
	    return _(e_cannot_open_termcap_file);
	if (i == 0)
# endif
#ifdef TERMINFO
	    return _(e_terminal_entry_not_found_in_terminfo);
#else
	    return _(e_terminal_entry_not_found_in_termcap);
#endif
    }
    return NULL;
}