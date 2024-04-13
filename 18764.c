ins_redraw(int ready)	    // not busy with something
{
#ifdef FEAT_CONCEAL
    linenr_T	conceal_old_cursor_line = 0;
    linenr_T	conceal_new_cursor_line = 0;
    int		conceal_update_lines = FALSE;
#endif

    if (char_avail())
	return;

    // Trigger CursorMoved if the cursor moved.  Not when the popup menu is
    // visible, the command might delete it.
    if (ready && (has_cursormovedI()
# ifdef FEAT_PROP_POPUP
		|| popup_visible
# endif
# if defined(FEAT_CONCEAL)
		|| curwin->w_p_cole > 0
# endif
		)
	    && !EQUAL_POS(last_cursormoved, curwin->w_cursor)
	    && !pum_visible())
    {
# ifdef FEAT_SYN_HL
	// Need to update the screen first, to make sure syntax
	// highlighting is correct after making a change (e.g., inserting
	// a "(".  The autocommand may also require a redraw, so it's done
	// again below, unfortunately.
	if (syntax_present(curwin) && must_redraw)
	    update_screen(0);
# endif
	if (has_cursormovedI())
	{
	    // Make sure curswant is correct, an autocommand may call
	    // getcurpos().
	    update_curswant();
	    ins_apply_autocmds(EVENT_CURSORMOVEDI);
	}
#ifdef FEAT_PROP_POPUP
	if (popup_visible)
	    popup_check_cursor_pos();
#endif
# ifdef FEAT_CONCEAL
	if (curwin->w_p_cole > 0)
	{
	    conceal_old_cursor_line = last_cursormoved.lnum;
	    conceal_new_cursor_line = curwin->w_cursor.lnum;
	    conceal_update_lines = TRUE;
	}
# endif
	last_cursormoved = curwin->w_cursor;
    }

    // Trigger TextChangedI if b_changedtick_i differs.
    if (ready && has_textchangedI()
	    && curbuf->b_last_changedtick_i != CHANGEDTICK(curbuf)
	    && !pum_visible())
    {
	aco_save_T	aco;
	varnumber_T	tick = CHANGEDTICK(curbuf);

	// save and restore curwin and curbuf, in case the autocmd changes them
	aucmd_prepbuf(&aco, curbuf);
	apply_autocmds(EVENT_TEXTCHANGEDI, NULL, NULL, FALSE, curbuf);
	aucmd_restbuf(&aco);
	curbuf->b_last_changedtick_i = CHANGEDTICK(curbuf);
	if (tick != CHANGEDTICK(curbuf))  // see ins_apply_autocmds()
	    u_save(curwin->w_cursor.lnum,
					(linenr_T)(curwin->w_cursor.lnum + 1));
    }

    // Trigger TextChangedP if b_changedtick_pum differs. When the popupmenu
    // closes TextChangedI will need to trigger for backwards compatibility,
    // thus use different b_last_changedtick* variables.
    if (ready && has_textchangedP()
	    && curbuf->b_last_changedtick_pum != CHANGEDTICK(curbuf)
	    && pum_visible())
    {
	aco_save_T	aco;
	varnumber_T	tick = CHANGEDTICK(curbuf);

	// save and restore curwin and curbuf, in case the autocmd changes them
	aucmd_prepbuf(&aco, curbuf);
	apply_autocmds(EVENT_TEXTCHANGEDP, NULL, NULL, FALSE, curbuf);
	aucmd_restbuf(&aco);
	curbuf->b_last_changedtick_pum = CHANGEDTICK(curbuf);
	if (tick != CHANGEDTICK(curbuf))  // see ins_apply_autocmds()
	    u_save(curwin->w_cursor.lnum,
					(linenr_T)(curwin->w_cursor.lnum + 1));
    }

    // Trigger SafeState if nothing is pending.
    may_trigger_safestate(ready
	    && !ins_compl_active()
	    && !pum_visible());

#if defined(FEAT_CONCEAL)
    if ((conceal_update_lines
	    && (conceal_old_cursor_line != conceal_new_cursor_line
		|| conceal_cursor_line(curwin)))
	    || need_cursor_line_redraw)
    {
	if (conceal_old_cursor_line != conceal_new_cursor_line)
	    redrawWinline(curwin, conceal_old_cursor_line);
	redrawWinline(curwin, conceal_new_cursor_line == 0
			    ? curwin->w_cursor.lnum : conceal_new_cursor_line);
	curwin->w_valid &= ~VALID_CROW;
	need_cursor_line_redraw = FALSE;
    }
#endif
    if (must_redraw)
	update_screen(0);
    else if (clear_cmdline || redraw_cmdline)
	showmode();		// clear cmdline and show mode
    showruler(FALSE);
    setcursor();
    emsg_on_display = FALSE;	// may remove error message now
}