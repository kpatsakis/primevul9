redraw_buf_and_status_later(buf_T *buf, int type)
{
    win_T	*wp;

#ifdef FEAT_WILDMENU
    if (wild_menu_showing != 0)
	// Don't redraw while the command line completion is displayed, it
	// would disappear.
	return;
#endif
    FOR_ALL_WINDOWS(wp)
    {
	if (wp->w_buffer == buf)
	{
	    redraw_win_later(wp, type);
	    wp->w_redr_status = TRUE;
	}
    }
}