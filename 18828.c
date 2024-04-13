handle_u7_response(int *arg, char_u *tp UNUSED, int csi_len UNUSED)
{
    if (arg[0] == 2 && arg[1] >= 2)
    {
	char *aw = NULL;

	LOG_TR(("Received U7 status: %s", tp));
	u7_status.tr_progress = STATUS_GOT;
	did_cursorhold = TRUE;
	if (arg[1] == 2)
	    aw = "single";
	else if (arg[1] == 3)
	    aw = "double";
	if (aw != NULL && STRCMP(aw, p_ambw) != 0)
	{
	    // Setting the option causes a screen redraw. Do
	    // that right away if possible, keeping any
	    // messages.
	    set_option_value_give_err((char_u *)"ambw", 0L, (char_u *)aw, 0);
# ifdef DEBUG_TERMRESPONSE
	    {
		int r = redraw_asap(CLEAR);

		log_tr("set 'ambiwidth', redraw_asap(): %d", r);
	    }
# else
	    redraw_asap(CLEAR);
# endif
# ifdef FEAT_EVAL
	    set_vim_var_string(VV_TERMU7RESP, tp, csi_len);
# endif
	}
    }
    else if (arg[0] == 3)
    {
	int value;

	LOG_TR(("Received compatibility test result: %s", tp));
	xcc_status.tr_progress = STATUS_GOT;

	// Third row: xterm compatibility test.
	// If the cursor is on the first column then the terminal can handle
	// the request for cursor style and blinking.
	value = arg[1] == 1 ? TPR_YES : TPR_NO;
	term_props[TPR_CURSOR_STYLE].tpr_status = value;
	term_props[TPR_CURSOR_BLINK].tpr_status = value;
    }
}