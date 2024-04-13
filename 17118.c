validate_virtcol_win(win_T *wp)
{
    check_cursor_moved(wp);
    if (!(wp->w_valid & VALID_VIRTCOL))
    {
	getvvcol(wp, &wp->w_cursor, NULL, &(wp->w_virtcol), NULL);
	wp->w_valid |= VALID_VIRTCOL;
#ifdef FEAT_SYN_HL
	if (wp->w_p_cuc && !pum_visible())
	    redraw_win_later(wp, SOME_VALID);
#endif
    }
}