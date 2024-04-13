settmode(tmode_T tmode)
{
#ifdef FEAT_GUI
    // don't set the term where gvim was started to any mode
    if (gui.in_use)
	return;
#endif

    if (full_screen)
    {
	/*
	 * When returning after calling a shell cur_tmode is TMODE_UNKNOWN,
	 * set the terminal to raw mode, even though we think it already is,
	 * because the shell program may have reset the terminal mode.
	 * When we think the terminal is normal, don't try to set it to
	 * normal again, because that causes problems (logout!) on some
	 * machines.
	 */
	if (tmode != cur_tmode)
	{
#ifdef FEAT_TERMRESPONSE
# ifdef FEAT_GUI
	    if (!gui.in_use && !gui.starting)
# endif
	    {
		// May need to check for T_CRV response and termcodes, it
		// doesn't work in Cooked mode, an external program may get
		// them.
		if (tmode != TMODE_RAW && termrequest_any_pending())
		    (void)vpeekc_nomap();
		check_for_codes_from_term();
	    }
#endif
	    if (tmode != TMODE_RAW)
		mch_setmouse(FALSE);	// switch mouse off

	    // Disable bracketed paste and modifyOtherKeys in cooked mode.
	    // Avoid doing this too often, on some terminals the codes are not
	    // handled properly.
	    if (termcap_active && tmode != TMODE_SLEEP
						   && cur_tmode != TMODE_SLEEP)
	    {
		MAY_WANT_TO_LOG_THIS;

		if (tmode != TMODE_RAW)
		{
		    out_str(T_BD);	// disable bracketed paste mode
		    out_str(T_CTE);	// possibly disables modifyOtherKeys
		}
		else
		{
		    out_str(T_BE);	// enable bracketed paste mode (should
					// be before mch_settmode().
		    out_str(T_CTI);	// possibly enables modifyOtherKeys
		}
	    }
	    out_flush();
	    mch_settmode(tmode);	// machine specific function
	    cur_tmode = tmode;
	    if (tmode == TMODE_RAW)
		setmouse();		// may switch mouse on
	    out_flush();
	}
#ifdef FEAT_TERMRESPONSE
	may_req_termresponse();
#endif
    }
}