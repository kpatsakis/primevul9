status_redraw_all(void)
{
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
	if (wp->w_status_height)
	{
	    wp->w_redr_status = TRUE;
	    redraw_later(VALID);
	}
}