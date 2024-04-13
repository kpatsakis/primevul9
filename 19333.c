may_adjust_incsearch_highlighting(
	int			firstc,
	long			count,
	incsearch_state_T	*is_state,
	int			c)
{
    int	    skiplen, patlen;
    pos_T   t;
    char_u  *pat;
    int	    search_flags = SEARCH_NOOF;
    int	    i;
    int	    save;
    int	    search_delim;

    // Parsing range may already set the last search pattern.
    // NOTE: must call restore_last_search_pattern() before returning!
    save_last_search_pattern();

    if (!do_incsearch_highlighting(firstc, &search_delim, is_state,
							    &skiplen, &patlen))
    {
	restore_last_search_pattern();
	return OK;
    }
    if (patlen == 0 && ccline.cmdbuff[skiplen] == NUL)
    {
	restore_last_search_pattern();
	return FAIL;
    }

    if (search_delim == ccline.cmdbuff[skiplen])
    {
	pat = last_search_pattern();
	if (pat == NULL)
	{
	    restore_last_search_pattern();
	    return FAIL;
	}
	skiplen = 0;
	patlen = (int)STRLEN(pat);
    }
    else
	pat = ccline.cmdbuff + skiplen;

    cursor_off();
    out_flush();
    if (c == Ctrl_G)
    {
	t = is_state->match_end;
	if (LT_POS(is_state->match_start, is_state->match_end))
	    // Start searching at the end of the match not at the beginning of
	    // the next column.
	    (void)decl(&t);
	search_flags += SEARCH_COL;
    }
    else
	t = is_state->match_start;
    if (!p_hls)
	search_flags += SEARCH_KEEP;
    ++emsg_off;
    save = pat[patlen];
    pat[patlen] = NUL;
    i = searchit(curwin, curbuf, &t, NULL,
		 c == Ctrl_G ? FORWARD : BACKWARD,
		 pat, count, search_flags, RE_SEARCH, NULL);
    --emsg_off;
    pat[patlen] = save;
    if (i)
    {
	is_state->search_start = is_state->match_start;
	is_state->match_end = t;
	is_state->match_start = t;
	if (c == Ctrl_T && firstc != '?')
	{
	    // Move just before the current match, so that when nv_search
	    // finishes the cursor will be put back on the match.
	    is_state->search_start = t;
	    (void)decl(&is_state->search_start);
	}
	else if (c == Ctrl_G && firstc == '?')
	{
	    // Move just after the current match, so that when nv_search
	    // finishes the cursor will be put back on the match.
	    is_state->search_start = t;
	    (void)incl(&is_state->search_start);
	}
	if (LT_POS(t, is_state->search_start) && c == Ctrl_G)
	{
	    // wrap around
	    is_state->search_start = t;
	    if (firstc == '?')
		(void)incl(&is_state->search_start);
	    else
		(void)decl(&is_state->search_start);
	}

	set_search_match(&is_state->match_end);
	curwin->w_cursor = is_state->match_start;
	changed_cline_bef_curs();
	update_topline();
	validate_cursor();
	highlight_match = TRUE;
	save_viewstate(&is_state->old_viewstate);
	update_screen(UPD_NOT_VALID);
	highlight_match = FALSE;
	redrawcmdline();
	curwin->w_cursor = is_state->match_end;
    }
    else
	vim_beep(BO_ERROR);
    restore_last_search_pattern();
    return FAIL;
}