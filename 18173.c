redraw_win_later(
    win_T	*wp,
    int		type)
{
    if (!exiting && wp->w_redr_type < type)
    {
	wp->w_redr_type = type;
	if (type >= NOT_VALID)
	    wp->w_lines_valid = 0;
	if (must_redraw < type)	// must_redraw is the maximum of all windows
	    must_redraw = type;
    }
}