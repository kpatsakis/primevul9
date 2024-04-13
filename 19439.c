nv_mousescroll(cmdarg_T *cap)
{
    win_T *old_curwin = curwin, *wp;

    if (mouse_row >= 0 && mouse_col >= 0)
    {
	int row, col;

	row = mouse_row;
	col = mouse_col;

	// find the window at the pointer coordinates
	wp = mouse_find_win(&row, &col, FIND_POPUP);
	if (wp == NULL)
	    return;
#ifdef FEAT_PROP_POPUP
	if (WIN_IS_POPUP(wp) && !wp->w_has_scrollbar)
	    return;
#endif
	curwin = wp;
	curbuf = curwin->w_buffer;
    }
    if (cap->arg == MSCR_UP || cap->arg == MSCR_DOWN)
    {
# ifdef FEAT_TERMINAL
	if (term_use_loop())
	    // This window is a terminal window, send the mouse event there.
	    // Set "typed" to FALSE to avoid an endless loop.
	    send_keys_to_term(curbuf->b_term, cap->cmdchar, mod_mask, FALSE);
	else
# endif
	if (mouse_vert_step < 0 || mod_mask & (MOD_MASK_SHIFT | MOD_MASK_CTRL))
	{
	    (void)onepage(cap->arg ? FORWARD : BACKWARD, 1L);
	}
	else
	{
	    // Don't scroll more than half the window height.
	    if (curwin->w_height < mouse_vert_step * 2)
	    {
		cap->count1 = curwin->w_height / 2;
		if (cap->count1 == 0)
		    cap->count1 = 1;
	    }
	    else
		cap->count1 = mouse_vert_step;
	    cap->count0 = cap->count1;
	    nv_scroll_line(cap);
	}
#ifdef FEAT_PROP_POPUP
	if (WIN_IS_POPUP(curwin))
	    popup_set_firstline(curwin);
#endif
    }
# ifdef FEAT_GUI
    else
    {
	// Horizontal scroll - only allowed when 'wrap' is disabled
	if (!curwin->w_p_wrap)
	{
	    int val, step;

	    if (mouse_hor_step < 0
		    || mod_mask & (MOD_MASK_SHIFT | MOD_MASK_CTRL))
		step = curwin->w_width;
	    else
		step = mouse_hor_step;
	    val = curwin->w_leftcol + (cap->arg == MSCR_RIGHT ? -step : +step);
	    if (val < 0)
		val = 0;

	    gui_do_horiz_scroll(val, TRUE);
	}
    }
# endif
# ifdef FEAT_SYN_HL
    if (curwin != old_curwin && curwin->w_p_cul)
	redraw_for_cursorline(curwin);
# endif
    may_trigger_winscrolled();

    curwin->w_redr_status = TRUE;

    curwin = old_curwin;
    curbuf = curwin->w_buffer;
}