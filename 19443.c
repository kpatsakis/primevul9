mouse_find_win(int *rowp, int *colp, mouse_find_T popup UNUSED)
{
    frame_T	*fp;
    win_T	*wp;

#ifdef FEAT_PROP_POPUP
    win_T	*pwp = NULL;

    if (popup != IGNORE_POPUP)
    {
	popup_reset_handled(POPUP_HANDLED_1);
	while ((wp = find_next_popup(TRUE, POPUP_HANDLED_1)) != NULL)
	{
	    if (*rowp >= wp->w_winrow && *rowp < wp->w_winrow + popup_height(wp)
		    && *colp >= wp->w_wincol
				    && *colp < wp->w_wincol + popup_width(wp))
		pwp = wp;
	}
	if (pwp != NULL)
	{
	    if (popup == FAIL_POPUP)
		return NULL;
	    *rowp -= pwp->w_winrow;
	    *colp -= pwp->w_wincol;
	    return pwp;
	}
    }
#endif

    fp = topframe;
    *rowp -= firstwin->w_winrow;
    for (;;)
    {
	if (fp->fr_layout == FR_LEAF)
	    break;
	if (fp->fr_layout == FR_ROW)
	{
	    for (fp = fp->fr_child; fp->fr_next != NULL; fp = fp->fr_next)
	    {
		if (*colp < fp->fr_width)
		    break;
		*colp -= fp->fr_width;
	    }
	}
	else    // fr_layout == FR_COL
	{
	    for (fp = fp->fr_child; fp->fr_next != NULL; fp = fp->fr_next)
	    {
		if (*rowp < fp->fr_height)
		    break;
		*rowp -= fp->fr_height;
	    }
	}
    }
    // When using a timer that closes a window the window might not actually
    // exist.
    FOR_ALL_WINDOWS(wp)
	if (wp == fp->fr_win)
	{
#ifdef FEAT_MENU
	    *rowp -= wp->w_winbar_height;
#endif
	    return wp;
	}
    return NULL;
}