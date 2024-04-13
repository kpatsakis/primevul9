ins_mousescroll(int dir)
{
    pos_T	tpos;
    win_T	*old_curwin = curwin, *wp;
    int		did_scroll = FALSE;

    tpos = curwin->w_cursor;

    if (mouse_row >= 0 && mouse_col >= 0)
    {
	int row, col;

	row = mouse_row;
	col = mouse_col;

	// find the window at the pointer coordinates
	wp = mouse_find_win(&row, &col, FIND_POPUP);
	if (wp == NULL)
	    return;
	curwin = wp;
	curbuf = curwin->w_buffer;
    }
    if (curwin == old_curwin)
	undisplay_dollar();

    // Don't scroll the window in which completion is being done.
    if (!pum_visible() || curwin != old_curwin)
    {
	long step;

	if (dir == MSCR_DOWN || dir == MSCR_UP)
	{
	    if (mouse_vert_step < 0
		    || mod_mask & (MOD_MASK_SHIFT | MOD_MASK_CTRL))
		step = (long)(curwin->w_botline - curwin->w_topline);
	    else
		step = mouse_vert_step;
	    scroll_redraw(dir, step);
# ifdef FEAT_PROP_POPUP
	if (WIN_IS_POPUP(curwin))
	    popup_set_firstline(curwin);
# endif
	}
#ifdef FEAT_GUI
	else
	{
	    int val;

	    if (mouse_hor_step < 0
		    || mod_mask & (MOD_MASK_SHIFT | MOD_MASK_CTRL))
		step = curwin->w_width;
	    else
		step = mouse_hor_step;
	    val = curwin->w_leftcol + (dir == MSCR_RIGHT ? -step : step);
	    if (val < 0)
		val = 0;
	    gui_do_horiz_scroll(val, TRUE);
	}
#endif
	did_scroll = TRUE;
	may_trigger_winscrolled();
    }

    curwin->w_redr_status = TRUE;

    curwin = old_curwin;
    curbuf = curwin->w_buffer;

    // The popup menu may overlay the window, need to redraw it.
    // TODO: Would be more efficient to only redraw the windows that are
    // overlapped by the popup menu.
    if (pum_visible() && did_scroll)
    {
	redraw_all_later(UPD_NOT_VALID);
	ins_compl_show_pum();
    }

    if (!EQUAL_POS(curwin->w_cursor, tpos))
    {
	start_arrow(&tpos);
	set_can_cindent(TRUE);
    }
}