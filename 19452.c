mouse_comp_pos(
    win_T	*win,
    int		*rowp,
    int		*colp,
    linenr_T	*lnump,
    int		*plines_cache)
{
    int		col = *colp;
    int		row = *rowp;
    linenr_T	lnum;
    int		retval = FALSE;
    int		off;
    int		count;

#ifdef FEAT_RIGHTLEFT
    if (win->w_p_rl)
	col = win->w_width - 1 - col;
#endif

    lnum = win->w_topline;

    while (row > 0)
    {
	int cache_idx = lnum - win->w_topline;

	// Only "Rows" lines are cached, with folding we'll run out of entries
	// and use the slow way.
	if (plines_cache != NULL && cache_idx < Rows
						&& plines_cache[cache_idx] > 0)
	    count = plines_cache[cache_idx];
	else
	{
#ifdef FEAT_DIFF
	    // Don't include filler lines in "count"
	    if (win->w_p_diff
# ifdef FEAT_FOLDING
		    && !hasFoldingWin(win, lnum, NULL, NULL, TRUE, NULL)
# endif
		    )
	    {
		if (lnum == win->w_topline)
		    row -= win->w_topfill;
		else
		    row -= diff_check_fill(win, lnum);
		count = plines_win_nofill(win, lnum, TRUE);
	    }
	    else
#endif
		count = plines_win(win, lnum, TRUE);
	    if (plines_cache != NULL && cache_idx < Rows)
		plines_cache[cache_idx] = count;
	}
	if (count > row)
	    break;	// Position is in this buffer line.
#ifdef FEAT_FOLDING
	(void)hasFoldingWin(win, lnum, NULL, &lnum, TRUE, NULL);
#endif
	if (lnum == win->w_buffer->b_ml.ml_line_count)
	{
	    retval = TRUE;
	    break;		// past end of file
	}
	row -= count;
	++lnum;
    }

    if (!retval)
    {
	// Compute the column without wrapping.
	off = win_col_off(win) - win_col_off2(win);
	if (col < off)
	    col = off;
	col += row * (win->w_width - off);
	// add skip column (for long wrapping line)
	col += win->w_skipcol;
    }

    if (!win->w_p_wrap)
	col += win->w_leftcol;

    // skip line number and fold column in front of the line
    col -= win_col_off(win);
    if (col <= 0)
    {
#ifdef FEAT_NETBEANS_INTG
	// if mouse is clicked on the gutter, then inform the netbeans server
	if (*colp < win_col_off(win))
	    netbeans_gutter_click(lnum);
#endif
	col = 0;
    }

    *colp = col;
    *rowp = row;
    *lnump = lnum;
    return retval;
}