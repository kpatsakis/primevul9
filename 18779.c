may_req_bg_color(void)
{
    if (can_get_termresponse() && starting == 0)
    {
	int didit = FALSE;

# ifdef FEAT_TERMINAL
	// Only request foreground if t_RF is set.
	if (rfg_status.tr_progress == STATUS_GET && *T_RFG != NUL)
	{
	    MAY_WANT_TO_LOG_THIS;
	    LOG_TR(("Sending FG request"));
	    out_str(T_RFG);
	    termrequest_sent(&rfg_status);
	    didit = TRUE;
	}
# endif

	// Only request background if t_RB is set.
	if (rbg_status.tr_progress == STATUS_GET && *T_RBG != NUL)
	{
	    MAY_WANT_TO_LOG_THIS;
	    LOG_TR(("Sending BG request"));
	    out_str(T_RBG);
	    termrequest_sent(&rbg_status);
	    didit = TRUE;
	}

	if (didit)
	{
	    // check for the characters now, otherwise they might be eaten by
	    // get_keystroke()
	    out_flush();
	    (void)vpeekc_nomap();
	}
    }
}