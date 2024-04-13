status_redraw_curbuf(void)
{
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
	if (wp->w_status_height != 0 && wp->w_buffer == curbuf)
	{
	    wp->w_redr_status = TRUE;
	    redraw_later(VALID);
	}
}