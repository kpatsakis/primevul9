set_shellsize(int width, int height, int mustset)
{
    static int		busy = FALSE;

    /*
     * Avoid recursiveness, can happen when setting the window size causes
     * another window-changed signal.
     */
    if (busy)
	return;

    if (width < 0 || height < 0)    // just checking...
	return;

    if (State == MODE_HITRETURN || State == MODE_SETWSIZE)
    {
	// postpone the resizing
	State = MODE_SETWSIZE;
	return;
    }

    if (updating_screen)
	// resizing while in update_screen() may cause a crash
	return;

    // curwin->w_buffer can be NULL when we are closing a window and the
    // buffer (or window) has already been closed and removing a scrollbar
    // causes a resize event. Don't resize then, it will happen after entering
    // another buffer.
    if (curwin->w_buffer == NULL || curwin->w_lines == NULL)
	return;

    ++busy;

#ifdef AMIGA
    out_flush();	    // must do this before mch_get_shellsize() for
			    // some obscure reason
#endif

    if (mustset || (ui_get_shellsize() == FAIL && height != 0))
    {
	Rows = height;
	Columns = width;
	check_shellsize();
	ui_set_shellsize(mustset);
    }
    else
	check_shellsize();

    // The window layout used to be adjusted here, but it now happens in
    // screenalloc() (also invoked from screenclear()).  That is because the
    // "busy" check above may skip this, but not screenalloc().

    if (State != MODE_ASKMORE && State != MODE_EXTERNCMD
						      && State != MODE_CONFIRM)
	screenclear();
    else
	screen_start();	    // don't know where cursor is now

    if (starting != NO_SCREEN)
    {
	maketitle();

	changed_line_abv_curs();
	invalidate_botline();

	/*
	 * We only redraw when it's needed:
	 * - While at the more prompt or executing an external command, don't
	 *   redraw, but position the cursor.
	 * - While editing the command line, only redraw that.
	 * - in Ex mode, don't redraw anything.
	 * - Otherwise, redraw right now, and position the cursor.
	 * Always need to call update_screen() or screenalloc(), to make
	 * sure Rows/Columns and the size of ScreenLines[] is correct!
	 */
	if (State == MODE_ASKMORE || State == MODE_EXTERNCMD
				     || State == MODE_CONFIRM || exmode_active)
	{
	    screenalloc(FALSE);
	    repeat_message();
	}
	else
	{
	    if (curwin->w_p_scb)
		do_check_scrollbind(TRUE);
	    if (State & MODE_CMDLINE)
	    {
		update_screen(NOT_VALID);
		redrawcmdline();
	    }
	    else
	    {
		update_topline();
		if (pum_visible())
		{
		    redraw_later(NOT_VALID);
		    ins_compl_show_pum();
		}
		update_screen(NOT_VALID);
		if (redrawing())
		    setcursor();
	    }
	}
	cursor_on();	    // redrawing may have switched it off
    }
    out_flush();
    --busy;
}