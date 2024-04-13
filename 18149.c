update_screen(int type_arg)
{
    int		type = type_arg;
    win_T	*wp;
    static int	did_intro = FALSE;
#ifdef FEAT_GUI
    int		did_one = FALSE;
    int		did_undraw = FALSE;
    int		gui_cursor_col = 0;
    int		gui_cursor_row = 0;
#endif
    int		no_update = FALSE;
    int		save_pum_will_redraw = pum_will_redraw;

    // Don't do anything if the screen structures are (not yet) valid.
    if (!screen_valid(TRUE))
	return FAIL;

    if (type == VALID_NO_UPDATE)
    {
	no_update = TRUE;
	type = 0;
    }

#ifdef FEAT_EVAL
    {
	buf_T *buf;

	// Before updating the screen, notify any listeners of changed text.
	FOR_ALL_BUFFERS(buf)
	    invoke_listeners(buf);
    }
#endif

#ifdef FEAT_DIFF
    // May have postponed updating diffs.
    if (need_diff_redraw)
	diff_redraw(TRUE);
#endif

    if (must_redraw)
    {
	if (type < must_redraw)	    // use maximal type
	    type = must_redraw;

	// must_redraw is reset here, so that when we run into some weird
	// reason to redraw while busy redrawing (e.g., asynchronous
	// scrolling), or update_topline() in win_update() will cause a
	// scroll, the screen will be redrawn later or in win_update().
	must_redraw = 0;
    }

    // May need to update w_lines[].
    if (curwin->w_lines_valid == 0 && type < NOT_VALID
#ifdef FEAT_TERMINAL
	    && !term_do_update_window(curwin)
#endif
		)
	type = NOT_VALID;

    // Postpone the redrawing when it's not needed and when being called
    // recursively.
    if (!redrawing() || updating_screen)
    {
	redraw_later(type);		// remember type for next time
	must_redraw = type;
	if (type > INVERTED_ALL)
	    curwin->w_lines_valid = 0;	// don't use w_lines[].wl_size now
	return FAIL;
    }
    updating_screen = TRUE;

#ifdef FEAT_PROP_POPUP
    // Update popup_mask if needed.  This may set w_redraw_top and w_redraw_bot
    // in some windows.
    may_update_popup_mask(type);
#endif

#ifdef FEAT_SYN_HL
    ++display_tick;	    // let syntax code know we're in a next round of
			    // display updating
#endif
    if (no_update)
	++no_win_do_lines_ins;

    // if the screen was scrolled up when displaying a message, scroll it down
    if (msg_scrolled)
    {
	clear_cmdline = TRUE;
	if (msg_scrolled > Rows - 5)	    // clearing is faster
	    type = CLEAR;
	else if (type != CLEAR)
	{
	    check_for_delay(FALSE);
	    if (screen_ins_lines(0, 0, msg_scrolled, (int)Rows, 0, NULL)
								       == FAIL)
		type = CLEAR;
	    FOR_ALL_WINDOWS(wp)
	    {
		if (wp->w_winrow < msg_scrolled)
		{
		    if (W_WINROW(wp) + wp->w_height > msg_scrolled
			    && wp->w_redr_type < REDRAW_TOP
			    && wp->w_lines_valid > 0
			    && wp->w_topline == wp->w_lines[0].wl_lnum)
		    {
			wp->w_upd_rows = msg_scrolled - W_WINROW(wp);
			wp->w_redr_type = REDRAW_TOP;
		    }
		    else
		    {
			wp->w_redr_type = NOT_VALID;
			if (W_WINROW(wp) + wp->w_height + wp->w_status_height
							       <= msg_scrolled)
			    wp->w_redr_status = TRUE;
		    }
		}
	    }
	    if (!no_update)
		redraw_cmdline = TRUE;
	    redraw_tabline = TRUE;
	}
	msg_scrolled = 0;
	need_wait_return = FALSE;
    }

    // reset cmdline_row now (may have been changed temporarily)
    compute_cmdrow();

    // Check for changed highlighting
    if (need_highlight_changed)
	highlight_changed();

    if (type == CLEAR)		// first clear screen
    {
	screenclear();		// will reset clear_cmdline
	type = NOT_VALID;
	// must_redraw may be set indirectly, avoid another redraw later
	must_redraw = 0;
    }

    if (clear_cmdline)		// going to clear cmdline (done below)
	check_for_delay(FALSE);

#ifdef FEAT_LINEBREAK
    // Force redraw when width of 'number' or 'relativenumber' column
    // changes.
    if (curwin->w_redr_type < NOT_VALID
	   && curwin->w_nrwidth != ((curwin->w_p_nu || curwin->w_p_rnu)
				    ? number_width(curwin) : 0))
	curwin->w_redr_type = NOT_VALID;
#endif

    // Only start redrawing if there is really something to do.
    if (type == INVERTED)
	update_curswant();
    if (curwin->w_redr_type < type
	    && !((type == VALID
		    && curwin->w_lines[0].wl_valid
#ifdef FEAT_DIFF
		    && curwin->w_topfill == curwin->w_old_topfill
		    && curwin->w_botfill == curwin->w_old_botfill
#endif
		    && curwin->w_topline == curwin->w_lines[0].wl_lnum)
		|| (type == INVERTED
		    && VIsual_active
		    && curwin->w_old_cursor_lnum == curwin->w_cursor.lnum
		    && curwin->w_old_visual_mode == VIsual_mode
		    && (curwin->w_valid & VALID_VIRTCOL)
		    && curwin->w_old_curswant == curwin->w_curswant)
		))
	curwin->w_redr_type = type;

    // Redraw the tab pages line if needed.
    if (redraw_tabline || type >= NOT_VALID)
	draw_tabline();

#ifdef FEAT_SYN_HL
    // Correct stored syntax highlighting info for changes in each displayed
    // buffer.  Each buffer must only be done once.
    FOR_ALL_WINDOWS(wp)
    {
	if (wp->w_buffer->b_mod_set)
	{
	    win_T	*wwp;

	    // Check if we already did this buffer.
	    for (wwp = firstwin; wwp != wp; wwp = wwp->w_next)
		if (wwp->w_buffer == wp->w_buffer)
		    break;
	    if (wwp == wp && syntax_present(wp))
		syn_stack_apply_changes(wp->w_buffer);
	}
    }
#endif

    if (pum_redraw_in_same_position())
	// Avoid flicker if the popup menu is going to be redrawn in the same
	// position.
	pum_will_redraw = TRUE;

    // Go from top to bottom through the windows, redrawing the ones that need
    // it.
#if defined(FEAT_SEARCH_EXTRA) || defined(FEAT_CLIPBOARD)
    did_update_one_window = FALSE;
#endif
#ifdef FEAT_SEARCH_EXTRA
    screen_search_hl.rm.regprog = NULL;
#endif
    FOR_ALL_WINDOWS(wp)
    {
	if (wp->w_redr_type != 0)
	{
	    cursor_off();
#ifdef FEAT_GUI
	    if (!did_one)
	    {
		did_one = TRUE;

		// Remove the cursor before starting to do anything, because
		// scrolling may make it difficult to redraw the text under
		// it.
		// Also remove the cursor if it needs to be hidden due to an
		// ongoing cursor-less sleep.
		if (gui.in_use && (wp == curwin || cursor_is_sleeping()))
		{
		    gui_cursor_col = gui.cursor_col;
		    gui_cursor_row = gui.cursor_row;
		    gui_undraw_cursor();
		    did_undraw = TRUE;
		}
	    }
#endif
	    win_update(wp);
	}

	// redraw status line after the window to minimize cursor movement
	if (wp->w_redr_status)
	{
	    cursor_off();
	    win_redr_status(wp, TRUE); // any popup menu will be redrawn below
	}
    }
#if defined(FEAT_SEARCH_EXTRA)
    end_search_hl();
#endif

    // May need to redraw the popup menu.
    pum_will_redraw = save_pum_will_redraw;
    pum_may_redraw();

    // Reset b_mod_set flags.  Going through all windows is probably faster
    // than going through all buffers (there could be many buffers).
    FOR_ALL_WINDOWS(wp)
	wp->w_buffer->b_mod_set = FALSE;

#ifdef FEAT_PROP_POPUP
    // Display popup windows on top of the windows and command line.
    update_popups(win_update);
#endif

    after_updating_screen(TRUE);

    // Clear or redraw the command line.  Done last, because scrolling may
    // mess up the command line.
    if (clear_cmdline || redraw_cmdline || redraw_mode)
	showmode();

    if (no_update)
	--no_win_do_lines_ins;

    // May put up an introductory message when not editing a file
    if (!did_intro)
	maybe_intro_message();
    did_intro = TRUE;

#ifdef FEAT_GUI
    // Redraw the cursor and update the scrollbars when all screen updating is
    // done.
    if (gui.in_use)
    {
	if (did_undraw && !gui_mch_is_blink_off())
	{
	    mch_disable_flush();
	    out_flush();	// required before updating the cursor
	    mch_enable_flush();

	    // Put the GUI position where the cursor was, gui_update_cursor()
	    // uses that.
	    gui.col = gui_cursor_col;
	    gui.row = gui_cursor_row;
	    gui.col = mb_fix_col(gui.col, gui.row);
	    gui_update_cursor(FALSE, FALSE);
	    gui_may_flush();
	    screen_cur_col = gui.col;
	    screen_cur_row = gui.row;
	}
	else
	    out_flush();
	gui_update_scrollbars(FALSE);
    }
#endif
    return OK;
}