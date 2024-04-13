stoptermcap(void)
{
    screen_stop_highlight();
    reset_cterm_colors();
    if (termcap_active)
    {
#ifdef FEAT_TERMRESPONSE
# ifdef FEAT_GUI
	if (!gui.in_use && !gui.starting)
# endif
	{
	    // May need to discard T_CRV, T_U7 or T_RBG response.
	    if (termrequest_any_pending())
	    {
# ifdef UNIX
		// Give the terminal a chance to respond.
		mch_delay(100L, 0);
# endif
# ifdef TCIFLUSH
		// Discard data received but not read.
		if (exiting)
		    tcflush(fileno(stdin), TCIFLUSH);
# endif
	    }
	    // Check for termcodes first, otherwise an external program may
	    // get them.
	    check_for_codes_from_term();
	}
#endif
	MAY_WANT_TO_LOG_THIS;

#if defined(UNIX) || defined(VMS)
	// Disable xterm's focus reporting mode if 'esckeys' is set.
	if (p_ek && *T_FD != NUL)
	    out_str(T_FD);
#endif

	out_str(T_BD);			// disable bracketed paste mode
	out_str(T_KE);			// stop "keypad transmit" mode
	out_flush();
	termcap_active = FALSE;
	cursor_on();			// just in case it is still off
	out_str(T_CTE);			// stop "raw" mode
	out_str(T_TE);			// stop termcap mode
	screen_start();			// don't know where cursor is now
	out_flush();
    }
}