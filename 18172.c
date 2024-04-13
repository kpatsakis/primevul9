redraw_custom_statusline(win_T *wp)
{
    static int	    entered = FALSE;
    int		    saved_did_emsg = did_emsg;

    // When called recursively return.  This can happen when the statusline
    // contains an expression that triggers a redraw.
    if (entered)
	return;
    entered = TRUE;

    did_emsg = FALSE;
    win_redr_custom(wp, FALSE);
    if (did_emsg)
    {
	// When there is an error disable the statusline, otherwise the
	// display is messed up with errors and a redraw triggers the problem
	// again and again.
	set_string_option_direct((char_u *)"statusline", -1,
		(char_u *)"", OPT_FREE | (*wp->w_p_stl != NUL
					? OPT_LOCAL : OPT_GLOBAL), SID_ERROR);
    }
    did_emsg |= saved_did_emsg;
    entered = FALSE;
}