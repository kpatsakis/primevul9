starttermcap(void)
{
    if (full_screen && !termcap_active)
    {
	MAY_WANT_TO_LOG_THIS;

	out_str(T_TI);			// start termcap mode
	out_str(T_CTI);			// start "raw" mode
	out_str(T_KS);			// start "keypad transmit" mode
	out_str(T_BE);			// enable bracketed paste mode

#if defined(UNIX) || defined(VMS)
	// Enable xterm's focus reporting mode when 'esckeys' is set.
	if (p_ek && *T_FE != NUL)
	    out_str(T_FE);
#endif

	out_flush();
	termcap_active = TRUE;
	screen_start();			// don't know where cursor is now
#ifdef FEAT_TERMRESPONSE
# ifdef FEAT_GUI
	if (!gui.in_use && !gui.starting)
# endif
	{
	    may_req_termresponse();
	    // Immediately check for a response.  If t_Co changes, we don't
	    // want to redraw with wrong colors first.
	    if (crv_status.tr_progress == STATUS_SENT)
		check_for_codes_from_term();
	}
#endif
    }
}