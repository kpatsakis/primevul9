may_do_incsearch_highlighting(
	int		    firstc,
	long		    count,
	incsearch_state_T   *is_state)
{
    int		skiplen, patlen;
    int		found;  // do_search() result
    pos_T	end_pos;
#ifdef FEAT_RELTIME
    searchit_arg_T sia;
#endif
    int		next_char;
    int		use_last_pat;
    int		did_do_incsearch = is_state->did_incsearch;
    int		search_delim;

    // Parsing range may already set the last search pattern.
    // NOTE: must call restore_last_search_pattern() before returning!
    save_last_search_pattern();

    if (!do_incsearch_highlighting(firstc, &search_delim, is_state,
							    &skiplen, &patlen))
    {
	restore_last_search_pattern();
	finish_incsearch_highlighting(FALSE, is_state, TRUE);
	if (did_do_incsearch && vpeekc() == NUL)
	    // may have skipped a redraw, do it now
	    redrawcmd();
	return;
    }

    // If there is a character waiting, search and redraw later.
    if (char_avail())
    {
	restore_last_search_pattern();
	is_state->incsearch_postponed = TRUE;
	return;
    }
    is_state->incsearch_postponed = FALSE;

    if (search_first_line == 0)
	// start at the original cursor position
	curwin->w_cursor = is_state->search_start;
    else if (search_first_line > curbuf->b_ml.ml_line_count)
    {
	// start after the last line
	curwin->w_cursor.lnum = curbuf->b_ml.ml_line_count;
	curwin->w_cursor.col = MAXCOL;
    }
    else
    {
	// start at the first line in the range
	curwin->w_cursor.lnum = search_first_line;
	curwin->w_cursor.col = 0;
    }

    // Use the previous pattern for ":s//".
    next_char = ccline.cmdbuff[skiplen + patlen];
    use_last_pat = patlen == 0 && skiplen > 0
				   && ccline.cmdbuff[skiplen - 1] == next_char;

    // If there is no pattern, don't do anything.
    if (patlen == 0 && !use_last_pat)
    {
	found = 0;
	set_no_hlsearch(TRUE); // turn off previous highlight
	redraw_all_later(UPD_SOME_VALID);
    }
    else
    {
	int search_flags = SEARCH_OPT + SEARCH_NOOF + SEARCH_PEEK;

	cursor_off();	// so the user knows we're busy
	out_flush();
	++emsg_off;	// so it doesn't beep if bad expr
	if (!p_hls)
	    search_flags += SEARCH_KEEP;
	if (search_first_line != 0)
	    search_flags += SEARCH_START;
	ccline.cmdbuff[skiplen + patlen] = NUL;
#ifdef FEAT_RELTIME
	CLEAR_FIELD(sia);
	// Set the time limit to half a second.
	sia.sa_tm = 500;
#endif
	found = do_search(NULL, firstc == ':' ? '/' : firstc, search_delim,
				 ccline.cmdbuff + skiplen, count, search_flags,
#ifdef FEAT_RELTIME
		&sia
#else
		NULL
#endif
		);
	ccline.cmdbuff[skiplen + patlen] = next_char;
	--emsg_off;

	if (curwin->w_cursor.lnum < search_first_line
		|| curwin->w_cursor.lnum > search_last_line)
	{
	    // match outside of address range
	    found = 0;
	    curwin->w_cursor = is_state->search_start;
	}

	// if interrupted while searching, behave like it failed
	if (got_int)
	{
	    (void)vpeekc();	// remove <C-C> from input stream
	    got_int = FALSE;	// don't abandon the command line
	    found = 0;
	}
	else if (char_avail())
	    // cancelled searching because a char was typed
	    is_state->incsearch_postponed = TRUE;
    }
    if (found != 0)
	highlight_match = TRUE;		// highlight position
    else
	highlight_match = FALSE;	// remove highlight

    // First restore the old curwin values, so the screen is positioned in the
    // same way as the actual search command.
    restore_viewstate(&is_state->old_viewstate);
    changed_cline_bef_curs();
    update_topline();

    if (found != 0)
    {
	pos_T	    save_pos = curwin->w_cursor;

	is_state->match_start = curwin->w_cursor;
	set_search_match(&curwin->w_cursor);
	validate_cursor();
	end_pos = curwin->w_cursor;
	is_state->match_end = end_pos;
	curwin->w_cursor = save_pos;
    }
    else
	end_pos = curwin->w_cursor; // shutup gcc 4

    // Disable 'hlsearch' highlighting if the pattern matches everything.
    // Avoids a flash when typing "foo\|".
    if (!use_last_pat)
    {
	next_char = ccline.cmdbuff[skiplen + patlen];
	ccline.cmdbuff[skiplen + patlen] = NUL;
	if (empty_pattern(ccline.cmdbuff + skiplen, search_delim)
							       && !no_hlsearch)
	{
	    redraw_all_later(UPD_SOME_VALID);
	    set_no_hlsearch(TRUE);
	}
	ccline.cmdbuff[skiplen + patlen] = next_char;
    }

    validate_cursor();
    // May redraw the status line to show the cursor position.
    if (p_ru && curwin->w_status_height > 0)
	curwin->w_redr_status = TRUE;

    update_screen(UPD_SOME_VALID);
    highlight_match = FALSE;
    restore_last_search_pattern();

    // Leave it at the end to make CTRL-R CTRL-W work.  But not when beyond the
    // end of the pattern, e.g. for ":s/pat/".
    if (ccline.cmdbuff[skiplen + patlen] != NUL)
	curwin->w_cursor = is_state->search_start;
    else if (found != 0)
	curwin->w_cursor = end_pos;

    msg_starthere();
    redrawcmdline();
    is_state->did_incsearch = TRUE;
}