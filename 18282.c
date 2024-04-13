ins_ctrl_ey(int tc)
{
    int	    c = tc;

    if (ctrl_x_mode_scroll())
    {
	if (c == Ctrl_Y)
	    scrolldown_clamp();
	else
	    scrollup_clamp();
	redraw_later(UPD_VALID);
    }
    else
    {
	c = ins_copychar(curwin->w_cursor.lnum + (c == Ctrl_Y ? -1 : 1));
	if (c != NUL)
	{
	    long	tw_save;

	    // The character must be taken literally, insert like it
	    // was typed after a CTRL-V, and pretend 'textwidth'
	    // wasn't set.  Digits, 'o' and 'x' are special after a
	    // CTRL-V, don't use it for these.
	    if (c < 256 && !isalnum(c))
		AppendToRedobuff((char_u *)CTRL_V_STR);	// CTRL-V
	    tw_save = curbuf->b_p_tw;
	    curbuf->b_p_tw = -1;
	    insert_special(c, TRUE, FALSE);
	    curbuf->b_p_tw = tw_save;
#ifdef FEAT_RIGHTLEFT
	    revins_chars++;
	    revins_legal++;
#endif
	    c = Ctrl_V;	// pretend CTRL-V is last character
	    auto_format(FALSE, TRUE);
	}
    }
    return c;
}