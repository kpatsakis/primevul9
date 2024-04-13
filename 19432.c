jump_to_mouse(
    int		flags,
    int		*inclusive,	// used for inclusive operator, can be NULL
    int		which_button)	// MOUSE_LEFT, MOUSE_RIGHT, MOUSE_MIDDLE
{
    static int	on_status_line = 0;	// #lines below bottom of window
    static int	on_sep_line = 0;	// on separator right of window
#ifdef FEAT_MENU
    static int  in_winbar = FALSE;
#endif
#ifdef FEAT_PROP_POPUP
    static int   in_popup_win = FALSE;
    static win_T *click_in_popup_win = NULL;
#endif
    static int	prev_row = -1;
    static int	prev_col = -1;
    static win_T *dragwin = NULL;	// window being dragged
    static int	did_drag = FALSE;	// drag was noticed

    win_T	*wp, *old_curwin;
    pos_T	old_cursor;
    int		count;
    int		first;
    int		row = mouse_row;
    int		col = mouse_col;
    colnr_T	col_from_screen = -1;
#ifdef FEAT_FOLDING
    int		mouse_char = ' ';
#endif

    mouse_past_bottom = FALSE;
    mouse_past_eol = FALSE;

    if (flags & MOUSE_RELEASED)
    {
	// On button release we may change window focus if positioned on a
	// status line and no dragging happened.
	if (dragwin != NULL && !did_drag)
	    flags &= ~(MOUSE_FOCUS | MOUSE_DID_MOVE);
	dragwin = NULL;
	did_drag = FALSE;
#ifdef FEAT_PROP_POPUP
	if (click_in_popup_win != NULL && popup_dragwin == NULL)
	    popup_close_for_mouse_click(click_in_popup_win);

	popup_dragwin = NULL;
	click_in_popup_win = NULL;
#endif
    }

    if ((flags & MOUSE_DID_MOVE)
	    && prev_row == mouse_row
	    && prev_col == mouse_col)
    {
retnomove:
	// before moving the cursor for a left click which is NOT in a status
	// line, stop Visual mode
	if (on_status_line)
	    return IN_STATUS_LINE;
	if (on_sep_line)
	    return IN_SEP_LINE;
#ifdef FEAT_MENU
	if (in_winbar)
	{
	    // A quick second click may arrive as a double-click, but we use it
	    // as a second click in the WinBar.
	    if ((mod_mask & MOD_MASK_MULTI_CLICK) && !(flags & MOUSE_RELEASED))
	    {
		wp = mouse_find_win(&row, &col, FAIL_POPUP);
		if (wp == NULL)
		    return IN_UNKNOWN;
		winbar_click(wp, col);
	    }
	    return IN_OTHER_WIN | MOUSE_WINBAR;
	}
#endif
	if (flags & MOUSE_MAY_STOP_VIS)
	{
	    end_visual_mode_keep_button();
	    redraw_curbuf_later(UPD_INVERTED);	// delete the inversion
	}
#if defined(FEAT_CMDWIN) && defined(FEAT_CLIPBOARD)
	// Continue a modeless selection in another window.
	if (cmdwin_type != 0 && row < curwin->w_winrow)
	    return IN_OTHER_WIN;
#endif
#ifdef FEAT_PROP_POPUP
	// Continue a modeless selection in a popup window or dragging it.
	if (in_popup_win)
	{
	    click_in_popup_win = NULL;  // don't close it on release
	    if (popup_dragwin != NULL)
	    {
		// dragging a popup window
		popup_drag(popup_dragwin);
		return IN_UNKNOWN;
	    }
	    return IN_OTHER_WIN;
	}
#endif
	return IN_BUFFER;
    }

    prev_row = mouse_row;
    prev_col = mouse_col;

    if (flags & MOUSE_SETPOS)
	goto retnomove;				// ugly goto...

    old_curwin = curwin;
    old_cursor = curwin->w_cursor;

    if (!(flags & MOUSE_FOCUS))
    {
	if (row < 0 || col < 0)			// check if it makes sense
	    return IN_UNKNOWN;

	// find the window where the row is in and adjust "row" and "col" to be
	// relative to top-left of the window
	wp = mouse_find_win(&row, &col, FIND_POPUP);
	if (wp == NULL)
	    return IN_UNKNOWN;
	dragwin = NULL;

#ifdef FEAT_PROP_POPUP
	// Click in a popup window may start dragging or modeless selection,
	// but not much else.
	if (WIN_IS_POPUP(wp))
	{
	    on_sep_line = 0;
	    on_status_line = 0;
	    in_popup_win = TRUE;
	    if (which_button == MOUSE_LEFT && popup_close_if_on_X(wp, row, col))
	    {
		return IN_UNKNOWN;
	    }
	    else if (((wp->w_popup_flags & (POPF_DRAG | POPF_RESIZE))
					      && popup_on_border(wp, row, col))
				       || (wp->w_popup_flags & POPF_DRAGALL))
	    {
		popup_dragwin = wp;
		popup_start_drag(wp, row, col);
		return IN_UNKNOWN;
	    }
	    // Only close on release, otherwise it's not possible to drag or do
	    // modeless selection.
	    else if (wp->w_popup_close == POPCLOSE_CLICK
		    && which_button == MOUSE_LEFT)
	    {
		click_in_popup_win = wp;
	    }
	    else if (which_button == MOUSE_LEFT)
		// If the click is in the scrollbar, may scroll up/down.
		popup_handle_scrollbar_click(wp, row, col);
# ifdef FEAT_CLIPBOARD
	    return IN_OTHER_WIN;
# else
	    return IN_UNKNOWN;
# endif
	}
	in_popup_win = FALSE;
	popup_dragwin = NULL;
#endif
#ifdef FEAT_MENU
	if (row == -1)
	{
	    // A click in the window toolbar does not enter another window or
	    // change Visual highlighting.
	    winbar_click(wp, col);
	    in_winbar = TRUE;
	    return IN_OTHER_WIN | MOUSE_WINBAR;
	}
	in_winbar = FALSE;
#endif

	// winpos and height may change in win_enter()!
	if (row >= wp->w_height)		// In (or below) status line
	{
	    on_status_line = row - wp->w_height + 1;
	    dragwin = wp;
	}
	else
	    on_status_line = 0;
	if (col >= wp->w_width)		// In separator line
	{
	    on_sep_line = col - wp->w_width + 1;
	    dragwin = wp;
	}
	else
	    on_sep_line = 0;

	// The rightmost character of the status line might be a vertical
	// separator character if there is no connecting window to the right.
	if (on_status_line && on_sep_line)
	{
	    if (stl_connected(wp))
		on_sep_line = 0;
	    else
		on_status_line = 0;
	}

	// Before jumping to another buffer, or moving the cursor for a left
	// click, stop Visual mode.
	if (VIsual_active
		&& (wp->w_buffer != curwin->w_buffer
		    || (!on_status_line && !on_sep_line
#ifdef FEAT_FOLDING
			&& (
# ifdef FEAT_RIGHTLEFT
			    wp->w_p_rl ? col < wp->w_width - wp->w_p_fdc :
# endif
			    col >= wp->w_p_fdc
# ifdef FEAT_CMDWIN
				  + (cmdwin_type == 0 && wp == curwin ? 0 : 1)
# endif
			    )
#endif
			&& (flags & MOUSE_MAY_STOP_VIS))))
	{
	    end_visual_mode_keep_button();
	    redraw_curbuf_later(UPD_INVERTED);	// delete the inversion
	}
#ifdef FEAT_CMDWIN
	if (cmdwin_type != 0 && wp != curwin)
	{
	    // A click outside the command-line window: Use modeless
	    // selection if possible.  Allow dragging the status lines.
	    on_sep_line = 0;
# ifdef FEAT_CLIPBOARD
	    if (on_status_line)
		return IN_STATUS_LINE;
	    return IN_OTHER_WIN;
# else
	    row = 0;
	    col += wp->w_wincol;
	    wp = curwin;
# endif
	}
#endif
#if defined(FEAT_PROP_POPUP) && defined(FEAT_TERMINAL)
	if (popup_is_popup(curwin) && curbuf->b_term != NULL)
	    // terminal in popup window: don't jump to another window
	    return IN_OTHER_WIN;
#endif
	// Only change window focus when not clicking on or dragging the
	// status line.  Do change focus when releasing the mouse button
	// (MOUSE_FOCUS was set above if we dragged first).
	if (dragwin == NULL || (flags & MOUSE_RELEASED))
	    win_enter(wp, TRUE);		// can make wp invalid!

	if (curwin != old_curwin)
	{
#ifdef CHECK_DOUBLE_CLICK
	    // set topline, to be able to check for double click ourselves
	    set_mouse_topline(curwin);
#endif
#ifdef FEAT_TERMINAL
	    // when entering a terminal window may change state
	    term_win_entered();
#endif
	}
	if (on_status_line)			// In (or below) status line
	{
	    // Don't use start_arrow() if we're in the same window
	    if (curwin == old_curwin)
		return IN_STATUS_LINE;
	    else
		return IN_STATUS_LINE | CURSOR_MOVED;
	}
	if (on_sep_line)			// In (or below) status line
	{
	    // Don't use start_arrow() if we're in the same window
	    if (curwin == old_curwin)
		return IN_SEP_LINE;
	    else
		return IN_SEP_LINE | CURSOR_MOVED;
	}

	curwin->w_cursor.lnum = curwin->w_topline;
#ifdef FEAT_GUI
	// remember topline, needed for double click
	gui_prev_topline = curwin->w_topline;
# ifdef FEAT_DIFF
	gui_prev_topfill = curwin->w_topfill;
# endif
#endif
    }
    else if (on_status_line && which_button == MOUSE_LEFT)
    {
	if (dragwin != NULL)
	{
	    // Drag the status line
	    count = row - W_WINROW(dragwin) - dragwin->w_height + 1
							     - on_status_line;
	    win_drag_status_line(dragwin, count);
	    did_drag |= count;
	}
	return IN_STATUS_LINE;			// Cursor didn't move
    }
    else if (on_sep_line && which_button == MOUSE_LEFT)
    {
	if (dragwin != NULL)
	{
	    // Drag the separator column
	    count = col - dragwin->w_wincol - dragwin->w_width + 1
								- on_sep_line;
	    win_drag_vsep_line(dragwin, count);
	    did_drag |= count;
	}
	return IN_SEP_LINE;			// Cursor didn't move
    }
#ifdef FEAT_MENU
    else if (in_winbar)
    {
	// After a click on the window toolbar don't start Visual mode.
	return IN_OTHER_WIN | MOUSE_WINBAR;
    }
#endif
    else // keep_window_focus must be TRUE
    {
	// before moving the cursor for a left click, stop Visual mode
	if (flags & MOUSE_MAY_STOP_VIS)
	{
	    end_visual_mode_keep_button();
	    redraw_curbuf_later(UPD_INVERTED);	// delete the inversion
	}

#if defined(FEAT_CMDWIN) && defined(FEAT_CLIPBOARD)
	// Continue a modeless selection in another window.
	if (cmdwin_type != 0 && row < curwin->w_winrow)
	    return IN_OTHER_WIN;
#endif
#ifdef FEAT_PROP_POPUP
	if (in_popup_win)
	{
	    if (popup_dragwin != NULL)
	    {
		// dragging a popup window
		popup_drag(popup_dragwin);
		return IN_UNKNOWN;
	    }
	    // continue a modeless selection in a popup window
	    click_in_popup_win = NULL;
	    return IN_OTHER_WIN;
	}
#endif

	row -= W_WINROW(curwin);
	col -= curwin->w_wincol;

	// When clicking beyond the end of the window, scroll the screen.
	// Scroll by however many rows outside the window we are.
	if (row < 0)
	{
	    count = 0;
	    for (first = TRUE; curwin->w_topline > 1; )
	    {
#ifdef FEAT_DIFF
		if (curwin->w_topfill < diff_check(curwin, curwin->w_topline))
		    ++count;
		else
#endif
		    count += plines(curwin->w_topline - 1);
		if (!first && count > -row)
		    break;
		first = FALSE;
#ifdef FEAT_FOLDING
		(void)hasFolding(curwin->w_topline, &curwin->w_topline, NULL);
#endif
#ifdef FEAT_DIFF
		if (curwin->w_topfill < diff_check(curwin, curwin->w_topline))
		    ++curwin->w_topfill;
		else
#endif
		{
		    --curwin->w_topline;
#ifdef FEAT_DIFF
		    curwin->w_topfill = 0;
#endif
		}
	    }
#ifdef FEAT_DIFF
	    check_topfill(curwin, FALSE);
#endif
	    curwin->w_valid &=
		      ~(VALID_WROW|VALID_CROW|VALID_BOTLINE|VALID_BOTLINE_AP);
	    redraw_later(UPD_VALID);
	    row = 0;
	}
	else if (row >= curwin->w_height)
	{
	    count = 0;
	    for (first = TRUE; curwin->w_topline < curbuf->b_ml.ml_line_count; )
	    {
#ifdef FEAT_DIFF
		if (curwin->w_topfill > 0)
		    ++count;
		else
#endif
		    count += plines(curwin->w_topline);
		if (!first && count > row - curwin->w_height + 1)
		    break;
		first = FALSE;
#ifdef FEAT_FOLDING
		if (hasFolding(curwin->w_topline, NULL, &curwin->w_topline)
			&& curwin->w_topline == curbuf->b_ml.ml_line_count)
		    break;
#endif
#ifdef FEAT_DIFF
		if (curwin->w_topfill > 0)
		    --curwin->w_topfill;
		else
#endif
		{
		    ++curwin->w_topline;
#ifdef FEAT_DIFF
		    curwin->w_topfill =
				   diff_check_fill(curwin, curwin->w_topline);
#endif
		}
	    }
#ifdef FEAT_DIFF
	    check_topfill(curwin, FALSE);
#endif
	    redraw_later(UPD_VALID);
	    curwin->w_valid &=
		      ~(VALID_WROW|VALID_CROW|VALID_BOTLINE|VALID_BOTLINE_AP);
	    row = curwin->w_height - 1;
	}
	else if (row == 0)
	{
	    // When dragging the mouse, while the text has been scrolled up as
	    // far as it goes, moving the mouse in the top line should scroll
	    // the text down (done later when recomputing w_topline).
	    if (mouse_dragging > 0
		    && curwin->w_cursor.lnum
				       == curwin->w_buffer->b_ml.ml_line_count
		    && curwin->w_cursor.lnum == curwin->w_topline)
		curwin->w_valid &= ~(VALID_TOPLINE);
	}
    }

    if (prev_row >= 0 && prev_row < Rows && prev_col >= 0 && prev_col <= Columns
						       && ScreenLines != NULL)
    {
	int off = LineOffset[prev_row] + prev_col;

	// Only use ScreenCols[] after the window was redrawn.  Mainly matters
	// for tests, a user would not click before redrawing.
	// Do not use when 'virtualedit' is active.
	if (curwin->w_redr_type <= UPD_VALID_NO_UPDATE && !virtual_active())
	    col_from_screen = ScreenCols[off];
#ifdef FEAT_FOLDING
	// Remember the character under the mouse, it might be a '-' or '+' in
	// the fold column.
	mouse_char = ScreenLines[off];
#endif
    }

#ifdef FEAT_FOLDING
    // Check for position outside of the fold column.
    if (
# ifdef FEAT_RIGHTLEFT
	    curwin->w_p_rl ? col < curwin->w_width - curwin->w_p_fdc :
# endif
	    col >= curwin->w_p_fdc
#  ifdef FEAT_CMDWIN
				+ (cmdwin_type == 0 ? 0 : 1)
#  endif
       )
	mouse_char = ' ';
#endif

    // compute the position in the buffer line from the posn on the screen
    if (mouse_comp_pos(curwin, &row, &col, &curwin->w_cursor.lnum, NULL))
	mouse_past_bottom = TRUE;

    // Start Visual mode before coladvance(), for when 'sel' != "old"
    if ((flags & MOUSE_MAY_VIS) && !VIsual_active)
    {
	check_visual_highlight();
	VIsual = old_cursor;
	VIsual_active = TRUE;
	VIsual_reselect = TRUE;
	// if 'selectmode' contains "mouse", start Select mode
	may_start_select('o');
	setmouse();
	if (p_smd && msg_silent == 0)
	    redraw_cmdline = TRUE;	// show visual mode later
    }

    if (col_from_screen >= 0)
    {
	// Use the column from ScreenCols[], it is accurate also after
	// concealed characters.
	curwin->w_cursor.col = col_from_screen;
	if (col_from_screen == MAXCOL)
	{
	    curwin->w_curswant = col_from_screen;
	    curwin->w_set_curswant = FALSE;	// May still have been TRUE
	    mouse_past_eol = TRUE;
	    if (inclusive != NULL)
		*inclusive = TRUE;
	}
	else
	{
	    curwin->w_set_curswant = TRUE;
	    if (inclusive != NULL)
		*inclusive = FALSE;
	}
	check_cursor_col();
    }
    else
    {
	curwin->w_curswant = col;
	curwin->w_set_curswant = FALSE;	// May still have been TRUE
	if (coladvance(col) == FAIL)	// Mouse click beyond end of line
	{
	    if (inclusive != NULL)
		*inclusive = TRUE;
	    mouse_past_eol = TRUE;
	}
	else if (inclusive != NULL)
	    *inclusive = FALSE;
    }

    count = IN_BUFFER;
    if (curwin != old_curwin || curwin->w_cursor.lnum != old_cursor.lnum
	    || curwin->w_cursor.col != old_cursor.col)
	count |= CURSOR_MOVED;		// Cursor has moved

# ifdef FEAT_FOLDING
    if (mouse_char == curwin->w_fill_chars.foldclosed)
	count |= MOUSE_FOLD_OPEN;
    else if (mouse_char != ' ')
	count |= MOUSE_FOLD_CLOSE;
# endif

    return count;
}