redraw_statuslines(void)
{
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
	if (wp->w_redr_status)
	    win_redr_status(wp, FALSE);
    if (redraw_tabline)
	draw_tabline();
}