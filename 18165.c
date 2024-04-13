win_update(win_T *wp)
{
    buf_T	*buf = wp->w_buffer;
    int		type;
    int		top_end = 0;	// Below last row of the top area that needs
				// updating.  0 when no top area updating.
    int		mid_start = 999;// first row of the mid area that needs
				// updating.  999 when no mid area updating.
    int		mid_end = 0;	// Below last row of the mid area that needs
				// updating.  0 when no mid area updating.
    int		bot_start = 999;// first row of the bot area that needs
				// updating.  999 when no bot area updating
    int		scrolled_down = FALSE;	// TRUE when scrolled down when
					// w_topline got smaller a bit
#ifdef FEAT_SEARCH_EXTRA
    int		top_to_mod = FALSE;    // redraw above mod_top
#endif

    int		row;		// current window row to display
    linenr_T	lnum;		// current buffer lnum to display
    int		idx;		// current index in w_lines[]
    int		srow;		// starting row of the current line

    int		eof = FALSE;	// if TRUE, we hit the end of the file
    int		didline = FALSE; // if TRUE, we finished the last line
    int		i;
    long	j;
    static int	recursive = FALSE;	// being called recursively
    linenr_T	old_botline = wp->w_botline;
#ifdef FEAT_CONCEAL
    int		old_wrow = wp->w_wrow;
    int		old_wcol = wp->w_wcol;
#endif
#ifdef FEAT_FOLDING
    long	fold_count;
#endif
#ifdef FEAT_SYN_HL
    // remember what happened to the previous line, to know if
    // check_visual_highlight() can be used
#define DID_NONE 1	// didn't update a line
#define DID_LINE 2	// updated a normal line
#define DID_FOLD 3	// updated a folded line
    int		did_update = DID_NONE;
    linenr_T	syntax_last_parsed = 0;		// last parsed text line
#endif
    linenr_T	mod_top = 0;
    linenr_T	mod_bot = 0;
#if defined(FEAT_SYN_HL) || defined(FEAT_SEARCH_EXTRA)
    int		save_got_int;
#endif
#ifdef SYN_TIME_LIMIT
    proftime_T	syntax_tm;
#endif

#if defined(FEAT_SEARCH_EXTRA) || defined(FEAT_CLIPBOARD)
    // This needs to be done only for the first window when update_screen() is
    // called.
    if (!did_update_one_window)
    {
	did_update_one_window = TRUE;
# ifdef FEAT_SEARCH_EXTRA
	start_search_hl();
# endif
# ifdef FEAT_CLIPBOARD
	// When Visual area changed, may have to update selection.
	if (clip_star.available && clip_isautosel_star())
	    clip_update_selection(&clip_star);
	if (clip_plus.available && clip_isautosel_plus())
	    clip_update_selection(&clip_plus);
# endif
    }
#endif

    type = wp->w_redr_type;

    if (type == NOT_VALID)
    {
	wp->w_redr_status = TRUE;
	wp->w_lines_valid = 0;
    }

    // Window frame is zero-height: nothing to draw.
    if (wp->w_height + WINBAR_HEIGHT(wp) == 0
	    || (wp->w_frame->fr_height == wp->w_status_height
#if defined(FEAT_PROP_POPUP)
		&& !popup_is_popup(wp)
#endif
	       ))
    {
	wp->w_redr_type = 0;
	return;
    }

    // Window is zero-width: Only need to draw the separator.
    if (wp->w_width == 0)
    {
	// draw the vertical separator right of this window
	draw_vsep_win(wp, 0);
	wp->w_redr_type = 0;
	return;
    }

#ifdef FEAT_TERMINAL
    // If this window contains a terminal, redraw works completely differently.
    if (term_do_update_window(wp))
    {
	term_update_window(wp);
# ifdef FEAT_MENU
	// Draw the window toolbar, if there is one.
	if (winbar_height(wp) > 0)
	    redraw_win_toolbar(wp);
# endif
	wp->w_redr_type = 0;
	return;
    }
#endif

#ifdef FEAT_SEARCH_EXTRA
    init_search_hl(wp, &screen_search_hl);
#endif

#ifdef FEAT_LINEBREAK
    // Force redraw when width of 'number' or 'relativenumber' column
    // changes.
    i = (wp->w_p_nu || wp->w_p_rnu) ? number_width(wp) : 0;
    if (wp->w_nrwidth != i)
    {
	type = NOT_VALID;
	wp->w_nrwidth = i;
    }
    else
#endif

    if (buf->b_mod_set && buf->b_mod_xlines != 0 && wp->w_redraw_top != 0)
    {
	// When there are both inserted/deleted lines and specific lines to be
	// redrawn, w_redraw_top and w_redraw_bot may be invalid, just redraw
	// everything (only happens when redrawing is off for while).
	type = NOT_VALID;
    }
    else
    {
	// Set mod_top to the first line that needs displaying because of
	// changes.  Set mod_bot to the first line after the changes.
	mod_top = wp->w_redraw_top;
	if (wp->w_redraw_bot != 0)
	    mod_bot = wp->w_redraw_bot + 1;
	else
	    mod_bot = 0;
	if (buf->b_mod_set)
	{
	    if (mod_top == 0 || mod_top > buf->b_mod_top)
	    {
		mod_top = buf->b_mod_top;
#ifdef FEAT_SYN_HL
		// Need to redraw lines above the change that may be included
		// in a pattern match.
		if (syntax_present(wp))
		{
		    mod_top -= buf->b_s.b_syn_sync_linebreaks;
		    if (mod_top < 1)
			mod_top = 1;
		}
#endif
	    }
	    if (mod_bot == 0 || mod_bot < buf->b_mod_bot)
		mod_bot = buf->b_mod_bot;

#ifdef FEAT_SEARCH_EXTRA
	    // When 'hlsearch' is on and using a multi-line search pattern, a
	    // change in one line may make the Search highlighting in a
	    // previous line invalid.  Simple solution: redraw all visible
	    // lines above the change.
	    // Same for a match pattern.
	    if (screen_search_hl.rm.regprog != NULL
		    && re_multiline(screen_search_hl.rm.regprog))
		top_to_mod = TRUE;
	    else
	    {
		matchitem_T *cur = wp->w_match_head;

		while (cur != NULL)
		{
		    if (cur->match.regprog != NULL
					   && re_multiline(cur->match.regprog))
		    {
			top_to_mod = TRUE;
			break;
		    }
		    cur = cur->next;
		}
	    }
#endif
	}
#ifdef FEAT_FOLDING
	if (mod_top != 0 && hasAnyFolding(wp))
	{
	    linenr_T	lnumt, lnumb;

	    // A change in a line can cause lines above it to become folded or
	    // unfolded.  Find the top most buffer line that may be affected.
	    // If the line was previously folded and displayed, get the first
	    // line of that fold.  If the line is folded now, get the first
	    // folded line.  Use the minimum of these two.

	    // Find last valid w_lines[] entry above mod_top.  Set lnumt to
	    // the line below it.  If there is no valid entry, use w_topline.
	    // Find the first valid w_lines[] entry below mod_bot.  Set lnumb
	    // to this line.  If there is no valid entry, use MAXLNUM.
	    lnumt = wp->w_topline;
	    lnumb = MAXLNUM;
	    for (i = 0; i < wp->w_lines_valid; ++i)
		if (wp->w_lines[i].wl_valid)
		{
		    if (wp->w_lines[i].wl_lastlnum < mod_top)
			lnumt = wp->w_lines[i].wl_lastlnum + 1;
		    if (lnumb == MAXLNUM && wp->w_lines[i].wl_lnum >= mod_bot)
		    {
			lnumb = wp->w_lines[i].wl_lnum;
			// When there is a fold column it might need updating
			// in the next line ("J" just above an open fold).
			if (compute_foldcolumn(wp, 0) > 0)
			    ++lnumb;
		    }
		}

	    (void)hasFoldingWin(wp, mod_top, &mod_top, NULL, TRUE, NULL);
	    if (mod_top > lnumt)
		mod_top = lnumt;

	    // Now do the same for the bottom line (one above mod_bot).
	    --mod_bot;
	    (void)hasFoldingWin(wp, mod_bot, NULL, &mod_bot, TRUE, NULL);
	    ++mod_bot;
	    if (mod_bot < lnumb)
		mod_bot = lnumb;
	}
#endif

	// When a change starts above w_topline and the end is below
	// w_topline, start redrawing at w_topline.
	// If the end of the change is above w_topline: do like no change was
	// made, but redraw the first line to find changes in syntax.
	if (mod_top != 0 && mod_top < wp->w_topline)
	{
	    if (mod_bot > wp->w_topline)
		mod_top = wp->w_topline;
#ifdef FEAT_SYN_HL
	    else if (syntax_present(wp))
		top_end = 1;
#endif
	}

	// When line numbers are displayed need to redraw all lines below
	// inserted/deleted lines.
	if (mod_top != 0 && buf->b_mod_xlines != 0 && wp->w_p_nu)
	    mod_bot = MAXLNUM;
    }
    wp->w_redraw_top = 0;	// reset for next time
    wp->w_redraw_bot = 0;

    // When only displaying the lines at the top, set top_end.  Used when
    // window has scrolled down for msg_scrolled.
    if (type == REDRAW_TOP)
    {
	j = 0;
	for (i = 0; i < wp->w_lines_valid; ++i)
	{
	    j += wp->w_lines[i].wl_size;
	    if (j >= wp->w_upd_rows)
	    {
		top_end = j;
		break;
	    }
	}
	if (top_end == 0)
	    // not found (cannot happen?): redraw everything
	    type = NOT_VALID;
	else
	    // top area defined, the rest is VALID
	    type = VALID;
    }

    // Trick: we want to avoid clearing the screen twice.  screenclear() will
    // set "screen_cleared" to TRUE.  The special value MAYBE (which is still
    // non-zero and thus not FALSE) will indicate that screenclear() was not
    // called.
    if (screen_cleared)
	screen_cleared = MAYBE;

    // If there are no changes on the screen that require a complete redraw,
    // handle three cases:
    // 1: we are off the top of the screen by a few lines: scroll down
    // 2: wp->w_topline is below wp->w_lines[0].wl_lnum: may scroll up
    // 3: wp->w_topline is wp->w_lines[0].wl_lnum: find first entry in
    //    w_lines[] that needs updating.
    if ((type == VALID || type == SOME_VALID
				  || type == INVERTED || type == INVERTED_ALL)
#ifdef FEAT_DIFF
	    && !wp->w_botfill && !wp->w_old_botfill
#endif
	    )
    {
	if (mod_top != 0
		&& wp->w_topline == mod_top
		&& (!wp->w_lines[0].wl_valid
		    || wp->w_topline <= wp->w_lines[0].wl_lnum))
	{
	    // w_topline is the first changed line and window is not scrolled,
	    // the scrolling from changed lines will be done further down.
	}
	else if (wp->w_lines[0].wl_valid
		&& (wp->w_topline < wp->w_lines[0].wl_lnum
#ifdef FEAT_DIFF
		    || (wp->w_topline == wp->w_lines[0].wl_lnum
			&& wp->w_topfill > wp->w_old_topfill)
#endif
		   ))
	{
	    // New topline is above old topline: May scroll down.
#ifdef FEAT_FOLDING
	    if (hasAnyFolding(wp))
	    {
		linenr_T ln;

		// count the number of lines we are off, counting a sequence
		// of folded lines as one
		j = 0;
		for (ln = wp->w_topline; ln < wp->w_lines[0].wl_lnum; ++ln)
		{
		    ++j;
		    if (j >= wp->w_height - 2)
			break;
		    (void)hasFoldingWin(wp, ln, NULL, &ln, TRUE, NULL);
		}
	    }
	    else
#endif
		j = wp->w_lines[0].wl_lnum - wp->w_topline;
	    if (j < wp->w_height - 2)		// not too far off
	    {
		i = plines_m_win(wp, wp->w_topline, wp->w_lines[0].wl_lnum - 1);
#ifdef FEAT_DIFF
		// insert extra lines for previously invisible filler lines
		if (wp->w_lines[0].wl_lnum != wp->w_topline)
		    i += diff_check_fill(wp, wp->w_lines[0].wl_lnum)
							  - wp->w_old_topfill;
#endif
		if (i < wp->w_height - 2)	// less than a screen off
		{
		    // Try to insert the correct number of lines.
		    // If not the last window, delete the lines at the bottom.
		    // win_ins_lines may fail when the terminal can't do it.
		    if (i > 0)
			check_for_delay(FALSE);
		    if (win_ins_lines(wp, 0, i, FALSE, wp == firstwin) == OK)
		    {
			if (wp->w_lines_valid != 0)
			{
			    // Need to update rows that are new, stop at the
			    // first one that scrolled down.
			    top_end = i;
			    scrolled_down = TRUE;

			    // Move the entries that were scrolled, disable
			    // the entries for the lines to be redrawn.
			    if ((wp->w_lines_valid += j) > wp->w_height)
				wp->w_lines_valid = wp->w_height;
			    for (idx = wp->w_lines_valid; idx - j >= 0; idx--)
				wp->w_lines[idx] = wp->w_lines[idx - j];
			    while (idx >= 0)
				wp->w_lines[idx--].wl_valid = FALSE;
			}
		    }
		    else
			mid_start = 0;		// redraw all lines
		}
		else
		    mid_start = 0;		// redraw all lines
	    }
	    else
		mid_start = 0;		// redraw all lines
	}
	else
	{
	    // New topline is at or below old topline: May scroll up.
	    // When topline didn't change, find first entry in w_lines[] that
	    // needs updating.

	    // try to find wp->w_topline in wp->w_lines[].wl_lnum
	    j = -1;
	    row = 0;
	    for (i = 0; i < wp->w_lines_valid; i++)
	    {
		if (wp->w_lines[i].wl_valid
			&& wp->w_lines[i].wl_lnum == wp->w_topline)
		{
		    j = i;
		    break;
		}
		row += wp->w_lines[i].wl_size;
	    }
	    if (j == -1)
	    {
		// if wp->w_topline is not in wp->w_lines[].wl_lnum redraw all
		// lines
		mid_start = 0;
	    }
	    else
	    {
		// Try to delete the correct number of lines.
		// wp->w_topline is at wp->w_lines[i].wl_lnum.
#ifdef FEAT_DIFF
		// If the topline didn't change, delete old filler lines,
		// otherwise delete filler lines of the new topline...
		if (wp->w_lines[0].wl_lnum == wp->w_topline)
		    row += wp->w_old_topfill;
		else
		    row += diff_check_fill(wp, wp->w_topline);
		// ... but don't delete new filler lines.
		row -= wp->w_topfill;
#endif
		if (row > 0)
		{
		    check_for_delay(FALSE);
		    if (win_del_lines(wp, 0, row, FALSE, wp == firstwin, 0)
									 == OK)
			bot_start = wp->w_height - row;
		    else
			mid_start = 0;		// redraw all lines
		}
		if ((row == 0 || bot_start < 999) && wp->w_lines_valid != 0)
		{
		    // Skip the lines (below the deleted lines) that are still
		    // valid and don't need redrawing.	Copy their info
		    // upwards, to compensate for the deleted lines.  Set
		    // bot_start to the first row that needs redrawing.
		    bot_start = 0;
		    idx = 0;
		    for (;;)
		    {
			wp->w_lines[idx] = wp->w_lines[j];
			// stop at line that didn't fit, unless it is still
			// valid (no lines deleted)
			if (row > 0 && bot_start + row
				 + (int)wp->w_lines[j].wl_size > wp->w_height)
			{
			    wp->w_lines_valid = idx + 1;
			    break;
			}
			bot_start += wp->w_lines[idx++].wl_size;

			// stop at the last valid entry in w_lines[].wl_size
			if (++j >= wp->w_lines_valid)
			{
			    wp->w_lines_valid = idx;
			    break;
			}
		    }
#ifdef FEAT_DIFF
		    // Correct the first entry for filler lines at the top
		    // when it won't get updated below.
		    if (wp->w_p_diff && bot_start > 0)
			wp->w_lines[0].wl_size =
			    plines_win_nofill(wp, wp->w_topline, TRUE)
							      + wp->w_topfill;
#endif
		}
	    }
	}

	// When starting redraw in the first line, redraw all lines.  When
	// there is only one window it's probably faster to clear the screen
	// first.
	if (mid_start == 0)
	{
	    mid_end = wp->w_height;
	    if (ONE_WINDOW && !WIN_IS_POPUP(wp))
	    {
		// Clear the screen when it was not done by win_del_lines() or
		// win_ins_lines() above, "screen_cleared" is FALSE or MAYBE
		// then.
		if (screen_cleared != TRUE)
		    screenclear();
		// The screen was cleared, redraw the tab pages line.
		if (redraw_tabline)
		    draw_tabline();
	    }
	}

	// When win_del_lines() or win_ins_lines() caused the screen to be
	// cleared (only happens for the first window) or when screenclear()
	// was called directly above, "must_redraw" will have been set to
	// NOT_VALID, need to reset it here to avoid redrawing twice.
	if (screen_cleared == TRUE)
	    must_redraw = 0;
    }
    else
    {
	// Not VALID or INVERTED: redraw all lines.
	mid_start = 0;
	mid_end = wp->w_height;
    }

    if (type == SOME_VALID)
    {
	// SOME_VALID: redraw all lines.
	mid_start = 0;
	mid_end = wp->w_height;
	type = NOT_VALID;
    }

    // check if we are updating or removing the inverted part
    if ((VIsual_active && buf == curwin->w_buffer)
	    || (wp->w_old_cursor_lnum != 0 && type != NOT_VALID))
    {
	linenr_T    from, to;

	if (VIsual_active)
	{
	    if (VIsual_active
		    && (VIsual_mode != wp->w_old_visual_mode
			|| type == INVERTED_ALL))
	    {
		// If the type of Visual selection changed, redraw the whole
		// selection.  Also when the ownership of the X selection is
		// gained or lost.
		if (curwin->w_cursor.lnum < VIsual.lnum)
		{
		    from = curwin->w_cursor.lnum;
		    to = VIsual.lnum;
		}
		else
		{
		    from = VIsual.lnum;
		    to = curwin->w_cursor.lnum;
		}
		// redraw more when the cursor moved as well
		if (wp->w_old_cursor_lnum < from)
		    from = wp->w_old_cursor_lnum;
		if (wp->w_old_cursor_lnum > to)
		    to = wp->w_old_cursor_lnum;
		if (wp->w_old_visual_lnum < from)
		    from = wp->w_old_visual_lnum;
		if (wp->w_old_visual_lnum > to)
		    to = wp->w_old_visual_lnum;
	    }
	    else
	    {
		// Find the line numbers that need to be updated: The lines
		// between the old cursor position and the current cursor
		// position.  Also check if the Visual position changed.
		if (curwin->w_cursor.lnum < wp->w_old_cursor_lnum)
		{
		    from = curwin->w_cursor.lnum;
		    to = wp->w_old_cursor_lnum;
		}
		else
		{
		    from = wp->w_old_cursor_lnum;
		    to = curwin->w_cursor.lnum;
		    if (from == 0)	// Visual mode just started
			from = to;
		}

		if (VIsual.lnum != wp->w_old_visual_lnum
					|| VIsual.col != wp->w_old_visual_col)
		{
		    if (wp->w_old_visual_lnum < from
						&& wp->w_old_visual_lnum != 0)
			from = wp->w_old_visual_lnum;
		    if (wp->w_old_visual_lnum > to)
			to = wp->w_old_visual_lnum;
		    if (VIsual.lnum < from)
			from = VIsual.lnum;
		    if (VIsual.lnum > to)
			to = VIsual.lnum;
		}
	    }

	    // If in block mode and changed column or curwin->w_curswant:
	    // update all lines.
	    // First compute the actual start and end column.
	    if (VIsual_mode == Ctrl_V)
	    {
		colnr_T	    fromc, toc;
#if defined(FEAT_LINEBREAK)
		int	    save_ve_flags = curwin->w_ve_flags;

		if (curwin->w_p_lbr)
		    curwin->w_ve_flags = VE_ALL;
#endif
		getvcols(wp, &VIsual, &curwin->w_cursor, &fromc, &toc);
		++toc;
#if defined(FEAT_LINEBREAK)
		curwin->w_ve_flags = save_ve_flags;
#endif
		// Highlight to the end of the line, unless 'virtualedit' has
		// "block".
		if (curwin->w_curswant == MAXCOL)
		{
		    if (get_ve_flags() & VE_BLOCK)
		    {
			pos_T	    pos;
			int	    cursor_above =
					   curwin->w_cursor.lnum < VIsual.lnum;

			// Need to find the longest line.
			toc = 0;
			pos.coladd = 0;
			for (pos.lnum = curwin->w_cursor.lnum; cursor_above
					? pos.lnum <= VIsual.lnum
					: pos.lnum >= VIsual.lnum;
					     pos.lnum += cursor_above ? 1 : -1)
			{
			    colnr_T t;

			    pos.col = (int)STRLEN(ml_get_buf(wp->w_buffer,
							     pos.lnum, FALSE));
			    getvvcol(wp, &pos, NULL, NULL, &t);
			    if (toc < t)
				toc = t;
			}
			++toc;
		    }
		    else
			toc = MAXCOL;
		}

		if (fromc != wp->w_old_cursor_fcol
			|| toc != wp->w_old_cursor_lcol)
		{
		    if (from > VIsual.lnum)
			from = VIsual.lnum;
		    if (to < VIsual.lnum)
			to = VIsual.lnum;
		}
		wp->w_old_cursor_fcol = fromc;
		wp->w_old_cursor_lcol = toc;
	    }
	}
	else
	{
	    // Use the line numbers of the old Visual area.
	    if (wp->w_old_cursor_lnum < wp->w_old_visual_lnum)
	    {
		from = wp->w_old_cursor_lnum;
		to = wp->w_old_visual_lnum;
	    }
	    else
	    {
		from = wp->w_old_visual_lnum;
		to = wp->w_old_cursor_lnum;
	    }
	}

	// There is no need to update lines above the top of the window.
	if (from < wp->w_topline)
	    from = wp->w_topline;

	// If we know the value of w_botline, use it to restrict the update to
	// the lines that are visible in the window.
	if (wp->w_valid & VALID_BOTLINE)
	{
	    if (from >= wp->w_botline)
		from = wp->w_botline - 1;
	    if (to >= wp->w_botline)
		to = wp->w_botline - 1;
	}

	// Find the minimal part to be updated.
	// Watch out for scrolling that made entries in w_lines[] invalid.
	// E.g., CTRL-U makes the first half of w_lines[] invalid and sets
	// top_end; need to redraw from top_end to the "to" line.
	// A middle mouse click with a Visual selection may change the text
	// above the Visual area and reset wl_valid, do count these for
	// mid_end (in srow).
	if (mid_start > 0)
	{
	    lnum = wp->w_topline;
	    idx = 0;
	    srow = 0;
	    if (scrolled_down)
		mid_start = top_end;
	    else
		mid_start = 0;
	    while (lnum < from && idx < wp->w_lines_valid)	// find start
	    {
		if (wp->w_lines[idx].wl_valid)
		    mid_start += wp->w_lines[idx].wl_size;
		else if (!scrolled_down)
		    srow += wp->w_lines[idx].wl_size;
		++idx;
# ifdef FEAT_FOLDING
		if (idx < wp->w_lines_valid && wp->w_lines[idx].wl_valid)
		    lnum = wp->w_lines[idx].wl_lnum;
		else
# endif
		    ++lnum;
	    }
	    srow += mid_start;
	    mid_end = wp->w_height;
	    for ( ; idx < wp->w_lines_valid; ++idx)		// find end
	    {
		if (wp->w_lines[idx].wl_valid
			&& wp->w_lines[idx].wl_lnum >= to + 1)
		{
		    // Only update until first row of this line
		    mid_end = srow;
		    break;
		}
		srow += wp->w_lines[idx].wl_size;
	    }
	}
    }

    if (VIsual_active && buf == curwin->w_buffer)
    {
	wp->w_old_visual_mode = VIsual_mode;
	wp->w_old_cursor_lnum = curwin->w_cursor.lnum;
	wp->w_old_visual_lnum = VIsual.lnum;
	wp->w_old_visual_col = VIsual.col;
	wp->w_old_curswant = curwin->w_curswant;
    }
    else
    {
	wp->w_old_visual_mode = 0;
	wp->w_old_cursor_lnum = 0;
	wp->w_old_visual_lnum = 0;
	wp->w_old_visual_col = 0;
    }

#if defined(FEAT_SYN_HL) || defined(FEAT_SEARCH_EXTRA)
    // reset got_int, otherwise regexp won't work
    save_got_int = got_int;
    got_int = 0;
#endif
#ifdef SYN_TIME_LIMIT
    // Set the time limit to 'redrawtime'.
    profile_setlimit(p_rdt, &syntax_tm);
    syn_set_timeout(&syntax_tm);
#endif
#ifdef FEAT_FOLDING
    win_foldinfo.fi_level = 0;
#endif

#ifdef FEAT_MENU
    // Draw the window toolbar, if there is one.
    // TODO: only when needed.
    if (winbar_height(wp) > 0)
	redraw_win_toolbar(wp);
#endif

    // Update all the window rows.
    idx = 0;		// first entry in w_lines[].wl_size
    row = 0;
    srow = 0;
    lnum = wp->w_topline;	// first line shown in window
    for (;;)
    {
	// stop updating when reached the end of the window (check for _past_
	// the end of the window is at the end of the loop)
	if (row == wp->w_height)
	{
	    didline = TRUE;
	    break;
	}

	// stop updating when hit the end of the file
	if (lnum > buf->b_ml.ml_line_count)
	{
	    eof = TRUE;
	    break;
	}

	// Remember the starting row of the line that is going to be dealt
	// with.  It is used further down when the line doesn't fit.
	srow = row;

	// Update a line when it is in an area that needs updating, when it
	// has changes or w_lines[idx] is invalid.
	// "bot_start" may be halfway a wrapped line after using
	// win_del_lines(), check if the current line includes it.
	// When syntax folding is being used, the saved syntax states will
	// already have been updated, we can't see where the syntax state is
	// the same again, just update until the end of the window.
	if (row < top_end
		|| (row >= mid_start && row < mid_end)
#ifdef FEAT_SEARCH_EXTRA
		|| top_to_mod
#endif
		|| idx >= wp->w_lines_valid
		|| (row + wp->w_lines[idx].wl_size > bot_start)
		|| (mod_top != 0
		    && (lnum == mod_top
			|| (lnum >= mod_top
			    && (lnum < mod_bot
#ifdef FEAT_SYN_HL
				|| did_update == DID_FOLD
				|| (did_update == DID_LINE
				    && syntax_present(wp)
				    && (
# ifdef FEAT_FOLDING
					(foldmethodIsSyntax(wp)
						      && hasAnyFolding(wp)) ||
# endif
					syntax_check_changed(lnum)))
#endif
#ifdef FEAT_SEARCH_EXTRA
				// match in fixed position might need redraw
				// if lines were inserted or deleted
				|| (wp->w_match_head != NULL
						    && buf->b_mod_xlines != 0)
#endif
				))))
#ifdef FEAT_SYN_HL
		|| (wp->w_p_cul && (lnum == wp->w_cursor.lnum
					     || lnum == wp->w_last_cursorline))
#endif
				)
	{
#ifdef FEAT_SEARCH_EXTRA
	    if (lnum == mod_top)
		top_to_mod = FALSE;
#endif

	    // When at start of changed lines: May scroll following lines
	    // up or down to minimize redrawing.
	    // Don't do this when the change continues until the end.
	    // Don't scroll when dollar_vcol >= 0, keep the "$".
	    // Don't scroll when redrawing the top, scrolled already above.
	    if (lnum == mod_top
		    && mod_bot != MAXLNUM
		    && !(dollar_vcol >= 0 && mod_bot == mod_top + 1)
		    && row >= top_end)
	    {
		int		old_rows = 0;
		int		new_rows = 0;
		int		xtra_rows;
		linenr_T	l;

		// Count the old number of window rows, using w_lines[], which
		// should still contain the sizes for the lines as they are
		// currently displayed.
		for (i = idx; i < wp->w_lines_valid; ++i)
		{
		    // Only valid lines have a meaningful wl_lnum.  Invalid
		    // lines are part of the changed area.
		    if (wp->w_lines[i].wl_valid
			    && wp->w_lines[i].wl_lnum == mod_bot)
			break;
		    old_rows += wp->w_lines[i].wl_size;
#ifdef FEAT_FOLDING
		    if (wp->w_lines[i].wl_valid
			    && wp->w_lines[i].wl_lastlnum + 1 == mod_bot)
		    {
			// Must have found the last valid entry above mod_bot.
			// Add following invalid entries.
			++i;
			while (i < wp->w_lines_valid
						  && !wp->w_lines[i].wl_valid)
			    old_rows += wp->w_lines[i++].wl_size;
			break;
		    }
#endif
		}

		if (i >= wp->w_lines_valid)
		{
		    // We can't find a valid line below the changed lines,
		    // need to redraw until the end of the window.
		    // Inserting/deleting lines has no use.
		    bot_start = 0;
		}
		else
		{
		    // Able to count old number of rows: Count new window
		    // rows, and may insert/delete lines
		    j = idx;
		    for (l = lnum; l < mod_bot; ++l)
		    {
#ifdef FEAT_FOLDING
			if (hasFoldingWin(wp, l, NULL, &l, TRUE, NULL))
			    ++new_rows;
			else
#endif
#ifdef FEAT_DIFF
			    if (l == wp->w_topline)
			    new_rows += plines_win_nofill(wp, l, TRUE)
							      + wp->w_topfill;
			else
#endif
			    new_rows += plines_win(wp, l, TRUE);
			++j;
			if (new_rows > wp->w_height - row - 2)
			{
			    // it's getting too much, must redraw the rest
			    new_rows = 9999;
			    break;
			}
		    }
		    xtra_rows = new_rows - old_rows;
		    if (xtra_rows < 0)
		    {
			// May scroll text up.  If there is not enough
			// remaining text or scrolling fails, must redraw the
			// rest.  If scrolling works, must redraw the text
			// below the scrolled text.
			if (row - xtra_rows >= wp->w_height - 2)
			    mod_bot = MAXLNUM;
			else
			{
			    check_for_delay(FALSE);
			    if (win_del_lines(wp, row,
					  -xtra_rows, FALSE, FALSE, 0) == FAIL)
				mod_bot = MAXLNUM;
			    else
				bot_start = wp->w_height + xtra_rows;
			}
		    }
		    else if (xtra_rows > 0)
		    {
			// May scroll text down.  If there is not enough
			// remaining text of scrolling fails, must redraw the
			// rest.
			if (row + xtra_rows >= wp->w_height - 2)
			    mod_bot = MAXLNUM;
			else
			{
			    check_for_delay(FALSE);
			    if (win_ins_lines(wp, row + old_rows,
					     xtra_rows, FALSE, FALSE) == FAIL)
				mod_bot = MAXLNUM;
			    else if (top_end > row + old_rows)
				// Scrolled the part at the top that requires
				// updating down.
				top_end += xtra_rows;
			}
		    }

		    // When not updating the rest, may need to move w_lines[]
		    // entries.
		    if (mod_bot != MAXLNUM && i != j)
		    {
			if (j < i)
			{
			    int x = row + new_rows;

			    // move entries in w_lines[] upwards
			    for (;;)
			    {
				// stop at last valid entry in w_lines[]
				if (i >= wp->w_lines_valid)
				{
				    wp->w_lines_valid = j;
				    break;
				}
				wp->w_lines[j] = wp->w_lines[i];
				// stop at a line that won't fit
				if (x + (int)wp->w_lines[j].wl_size
							   > wp->w_height)
				{
				    wp->w_lines_valid = j + 1;
				    break;
				}
				x += wp->w_lines[j++].wl_size;
				++i;
			    }
			    if (bot_start > x)
				bot_start = x;
			}
			else // j > i
			{
			    // move entries in w_lines[] downwards
			    j -= i;
			    wp->w_lines_valid += j;
			    if (wp->w_lines_valid > wp->w_height)
				wp->w_lines_valid = wp->w_height;
			    for (i = wp->w_lines_valid; i - j >= idx; --i)
				wp->w_lines[i] = wp->w_lines[i - j];

			    // The w_lines[] entries for inserted lines are
			    // now invalid, but wl_size may be used above.
			    // Reset to zero.
			    while (i >= idx)
			    {
				wp->w_lines[i].wl_size = 0;
				wp->w_lines[i--].wl_valid = FALSE;
			    }
			}
		    }
		}
	    }

#ifdef FEAT_FOLDING
	    // When lines are folded, display one line for all of them.
	    // Otherwise, display normally (can be several display lines when
	    // 'wrap' is on).
	    fold_count = foldedCount(wp, lnum, &win_foldinfo);
	    if (fold_count != 0)
	    {
		fold_line(wp, fold_count, &win_foldinfo, lnum, row);
		++row;
		--fold_count;
		wp->w_lines[idx].wl_folded = TRUE;
		wp->w_lines[idx].wl_lastlnum = lnum + fold_count;
# ifdef FEAT_SYN_HL
		did_update = DID_FOLD;
# endif
	    }
	    else
#endif
	    if (idx < wp->w_lines_valid
		    && wp->w_lines[idx].wl_valid
		    && wp->w_lines[idx].wl_lnum == lnum
		    && lnum > wp->w_topline
		    && !(dy_flags & (DY_LASTLINE | DY_TRUNCATE))
		    && !WIN_IS_POPUP(wp)
		    && srow + wp->w_lines[idx].wl_size > wp->w_height
#ifdef FEAT_DIFF
		    && diff_check_fill(wp, lnum) == 0
#endif
		    )
	    {
		// This line is not going to fit.  Don't draw anything here,
		// will draw "@  " lines below.
		row = wp->w_height + 1;
	    }
	    else
	    {
#ifdef FEAT_SEARCH_EXTRA
		prepare_search_hl(wp, &screen_search_hl, lnum);
#endif
#ifdef FEAT_SYN_HL
		// Let the syntax stuff know we skipped a few lines.
		if (syntax_last_parsed != 0 && syntax_last_parsed + 1 < lnum
						       && syntax_present(wp))
		    syntax_end_parsing(syntax_last_parsed + 1);
#endif

		// Display one line.
		row = win_line(wp, lnum, srow, wp->w_height,
							  mod_top == 0, FALSE);

#ifdef FEAT_FOLDING
		wp->w_lines[idx].wl_folded = FALSE;
		wp->w_lines[idx].wl_lastlnum = lnum;
#endif
#ifdef FEAT_SYN_HL
		did_update = DID_LINE;
		syntax_last_parsed = lnum;
#endif
	    }

	    wp->w_lines[idx].wl_lnum = lnum;
	    wp->w_lines[idx].wl_valid = TRUE;

	    // Past end of the window or end of the screen. Note that after
	    // resizing wp->w_height may be end up too big. That's a problem
	    // elsewhere, but prevent a crash here.
	    if (row > wp->w_height || row + wp->w_winrow >= Rows)
	    {
		// we may need the size of that too long line later on
		if (dollar_vcol == -1)
		    wp->w_lines[idx].wl_size = plines_win(wp, lnum, TRUE);
		++idx;
		break;
	    }
	    if (dollar_vcol == -1)
		wp->w_lines[idx].wl_size = row - srow;
	    ++idx;
#ifdef FEAT_FOLDING
	    lnum += fold_count + 1;
#else
	    ++lnum;
#endif
	}
	else
	{
	    if (wp->w_p_rnu)
	    {
#ifdef FEAT_FOLDING
		// 'relativenumber' set: The text doesn't need to be drawn, but
		// the number column nearly always does.
		fold_count = foldedCount(wp, lnum, &win_foldinfo);
		if (fold_count != 0)
		    fold_line(wp, fold_count, &win_foldinfo, lnum, row);
		else
#endif
		    (void)win_line(wp, lnum, srow, wp->w_height, TRUE, TRUE);
	    }

	    // This line does not need to be drawn, advance to the next one.
	    row += wp->w_lines[idx++].wl_size;
	    if (row > wp->w_height)	// past end of screen
		break;
#ifdef FEAT_FOLDING
	    lnum = wp->w_lines[idx - 1].wl_lastlnum + 1;
#else
	    ++lnum;
#endif
#ifdef FEAT_SYN_HL
	    did_update = DID_NONE;
#endif
	}

	if (lnum > buf->b_ml.ml_line_count)
	{
	    eof = TRUE;
	    break;
	}
    }

    // End of loop over all window lines.

#ifdef FEAT_VTP
    // Rewrite the character at the end of the screen line.
    // See the version that was fixed.
    if (use_vtp() && get_conpty_fix_type() < 1)
    {
	int i;

	for (i = 0; i < Rows; ++i)
	    if (enc_utf8)
		if ((*mb_off2cells)(LineOffset[i] + Columns - 2,
					   LineOffset[i] + screen_Columns) > 1)
		    screen_draw_rectangle(i, Columns - 2, 1, 2, FALSE);
		else
		    screen_draw_rectangle(i, Columns - 1, 1, 1, FALSE);
	    else
		screen_char(LineOffset[i] + Columns - 1, i, Columns - 1);
    }
#endif

    if (idx > wp->w_lines_valid)
	wp->w_lines_valid = idx;

#ifdef FEAT_SYN_HL
    // Let the syntax stuff know we stop parsing here.
    if (syntax_last_parsed != 0 && syntax_present(wp))
	syntax_end_parsing(syntax_last_parsed + 1);
#endif

    // If we didn't hit the end of the file, and we didn't finish the last
    // line we were working on, then the line didn't fit.
    wp->w_empty_rows = 0;
#ifdef FEAT_DIFF
    wp->w_filler_rows = 0;
#endif
    if (!eof && !didline)
    {
	if (lnum == wp->w_topline)
	{
	    // Single line that does not fit!
	    // Don't overwrite it, it can be edited.
	    wp->w_botline = lnum + 1;
	}
#ifdef FEAT_DIFF
	else if (diff_check_fill(wp, lnum) >= wp->w_height - srow)
	{
	    // Window ends in filler lines.
	    wp->w_botline = lnum;
	    wp->w_filler_rows = wp->w_height - srow;
	}
#endif
#ifdef FEAT_PROP_POPUP
	else if (WIN_IS_POPUP(wp))
	{
	    // popup line that doesn't fit is left as-is
	    wp->w_botline = lnum;
	}
#endif
	else if (dy_flags & DY_TRUNCATE)	// 'display' has "truncate"
	{
	    int scr_row = W_WINROW(wp) + wp->w_height - 1;

	    // Last line isn't finished: Display "@@@" in the last screen line.
	    screen_puts_len((char_u *)"@@", 2, scr_row, wp->w_wincol,
							      HL_ATTR(HLF_AT));
	    screen_fill(scr_row, scr_row + 1,
		    (int)wp->w_wincol + 2, (int)W_ENDCOL(wp),
		    '@', ' ', HL_ATTR(HLF_AT));
	    set_empty_rows(wp, srow);
	    wp->w_botline = lnum;
	}
	else if (dy_flags & DY_LASTLINE)	// 'display' has "lastline"
	{
	    // Last line isn't finished: Display "@@@" at the end.
	    screen_fill(W_WINROW(wp) + wp->w_height - 1,
		    W_WINROW(wp) + wp->w_height,
		    (int)W_ENDCOL(wp) - 3, (int)W_ENDCOL(wp),
		    '@', '@', HL_ATTR(HLF_AT));
	    set_empty_rows(wp, srow);
	    wp->w_botline = lnum;
	}
	else
	{
	    win_draw_end(wp, '@', ' ', TRUE, srow, wp->w_height, HLF_AT);
	    wp->w_botline = lnum;
	}
    }
    else
    {
	draw_vsep_win(wp, row);
	if (eof)		// we hit the end of the file
	{
	    wp->w_botline = buf->b_ml.ml_line_count + 1;
#ifdef FEAT_DIFF
	    j = diff_check_fill(wp, wp->w_botline);
	    if (j > 0 && !wp->w_botfill)
	    {
		// Display filler lines at the end of the file.
		if (char2cells(fill_diff) > 1)
		    i = '-';
		else
		    i = fill_diff;
		if (row + j > wp->w_height)
		    j = wp->w_height - row;
		win_draw_end(wp, i, i, TRUE, row, row + (int)j, HLF_DED);
		row += j;
	    }
#endif
	}
	else if (dollar_vcol == -1)
	    wp->w_botline = lnum;

	// Make sure the rest of the screen is blank
	// write the 'fill_eob' character to rows that aren't part of the file
	if (WIN_IS_POPUP(wp))
	    win_draw_end(wp, ' ', ' ', FALSE, row, wp->w_height, HLF_AT);
	else
	    win_draw_end(wp, fill_eob, ' ', FALSE, row, wp->w_height, HLF_EOB);
    }

#ifdef SYN_TIME_LIMIT
    syn_set_timeout(NULL);
#endif

    // Reset the type of redrawing required, the window has been updated.
    wp->w_redr_type = 0;
#ifdef FEAT_DIFF
    wp->w_old_topfill = wp->w_topfill;
    wp->w_old_botfill = wp->w_botfill;
#endif

    if (dollar_vcol == -1)
    {
	// There is a trick with w_botline.  If we invalidate it on each
	// change that might modify it, this will cause a lot of expensive
	// calls to plines() in update_topline() each time.  Therefore the
	// value of w_botline is often approximated, and this value is used to
	// compute the value of w_topline.  If the value of w_botline was
	// wrong, check that the value of w_topline is correct (cursor is on
	// the visible part of the text).  If it's not, we need to redraw
	// again.  Mostly this just means scrolling up a few lines, so it
	// doesn't look too bad.  Only do this for the current window (where
	// changes are relevant).
	wp->w_valid |= VALID_BOTLINE;
	if (wp == curwin && wp->w_botline != old_botline && !recursive)
	{
	    win_T	*wwp;
#if defined(FEAT_CONCEAL)
	    linenr_T	old_topline = wp->w_topline;
	    int		new_wcol = wp->w_wcol;
#endif
	    recursive = TRUE;
	    curwin->w_valid &= ~VALID_TOPLINE;
	    update_topline();	// may invalidate w_botline again

#if defined(FEAT_CONCEAL)
	    if (old_wcol != new_wcol && (wp->w_valid & (VALID_WCOL|VALID_WROW))
						    != (VALID_WCOL|VALID_WROW))
	    {
		// A win_line() call applied a fix to screen cursor column to
		// accommodate concealment of cursor line, but in this call to
		// update_topline() the cursor's row or column got invalidated.
		// If they are left invalid, setcursor() will recompute them
		// but there won't be any further win_line() call to re-fix the
		// column and the cursor will end up misplaced.  So we call
		// cursor validation now and reapply the fix again (or call
		// win_line() to do it for us).
		validate_cursor();
		if (wp->w_wcol == old_wcol && wp->w_wrow == old_wrow
					       && old_topline == wp->w_topline)
		    wp->w_wcol = new_wcol;
		else
		    redrawWinline(wp, wp->w_cursor.lnum);
	    }
#endif
	    // New redraw either due to updated topline or due to wcol fix.
	    if (wp->w_redr_type != 0)
	    {
		// Don't update for changes in buffer again.
		i = curbuf->b_mod_set;
		curbuf->b_mod_set = FALSE;
		j = curbuf->b_mod_xlines;
		curbuf->b_mod_xlines = 0;
		win_update(curwin);
		curbuf->b_mod_set = i;
		curbuf->b_mod_xlines = j;
	    }
	    // Other windows might have w_redr_type raised in update_topline().
	    must_redraw = 0;
	    FOR_ALL_WINDOWS(wwp)
		if (wwp->w_redr_type > must_redraw)
		    must_redraw = wwp->w_redr_type;
	    recursive = FALSE;
	}
    }

#if defined(FEAT_SYN_HL) || defined(FEAT_SEARCH_EXTRA)
    // restore got_int, unless CTRL-C was hit while redrawing
    if (!got_int)
	got_int = save_got_int;
#endif
}