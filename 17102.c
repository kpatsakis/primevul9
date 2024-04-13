redraw_for_cursorline(win_T *wp)
{
    if ((wp->w_p_rnu
#ifdef FEAT_SYN_HL
		|| wp->w_p_cul
#endif
		)
	    && (wp->w_valid & VALID_CROW) == 0
	    && !pum_visible())
    {
	if (wp->w_p_rnu)
	    // win_line() will redraw the number column only.
	    redraw_win_later(wp, VALID);
#ifdef FEAT_SYN_HL
	if (wp->w_p_cul)
	{
	    if (wp->w_redr_type <= VALID && wp->w_last_cursorline != 0)
	    {
		// "w_last_cursorline" may be outdated, worst case we redraw
		// too much.  This is optimized for moving the cursor around in
		// the current window.
		redrawWinline(wp, wp->w_last_cursorline);
		redrawWinline(wp, wp->w_cursor.lnum);
	    }
	    else
		redraw_win_later(wp, SOME_VALID);
	}
#endif
    }
}