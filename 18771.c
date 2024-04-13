may_adjust_color_count(int val)
{
    if (val != t_colors)
    {
	// Nr of colors changed, initialize highlighting and
	// redraw everything.  This causes a redraw, which usually
	// clears the message.  Try keeping the message if it
	// might work.
	set_keep_msg_from_hist();
	set_color_count(val);
	init_highlight(TRUE, FALSE);
# ifdef DEBUG_TERMRESPONSE
	{
	    int r = redraw_asap(CLEAR);

	    log_tr("Received t_Co, redraw_asap(): %d", r);
	}
# else
	redraw_asap(CLEAR);
# endif
    }
}