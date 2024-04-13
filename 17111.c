scroll_cursor_bot(int min_scroll, int set_topbot)
{
    int		used;
    int		scrolled = 0;
    int		extra = 0;
    int		i;
    linenr_T	line_count;
    linenr_T	old_topline = curwin->w_topline;
    lineoff_T	loff;
    lineoff_T	boff;
#ifdef FEAT_DIFF
    int		old_topfill = curwin->w_topfill;
    int		fill_below_window;
#endif
    linenr_T	old_botline = curwin->w_botline;
    linenr_T	old_valid = curwin->w_valid;
    int		old_empty_rows = curwin->w_empty_rows;
    linenr_T	cln;		    // Cursor Line Number
    long        so = get_scrolloff_value();

    cln = curwin->w_cursor.lnum;
    if (set_topbot)
    {
	used = 0;
	curwin->w_botline = cln + 1;
#ifdef FEAT_DIFF
	loff.fill = 0;
#endif
	for (curwin->w_topline = curwin->w_botline;
		curwin->w_topline > 1;
		curwin->w_topline = loff.lnum)
	{
	    loff.lnum = curwin->w_topline;
	    topline_back(&loff);
	    if (loff.height == MAXCOL || used + loff.height > curwin->w_height)
		break;
	    used += loff.height;
#ifdef FEAT_DIFF
	    curwin->w_topfill = loff.fill;
#endif
	}
	set_empty_rows(curwin, used);
	curwin->w_valid |= VALID_BOTLINE|VALID_BOTLINE_AP;
	if (curwin->w_topline != old_topline
#ifdef FEAT_DIFF
		|| curwin->w_topfill != old_topfill
#endif
		)
	    curwin->w_valid &= ~(VALID_WROW|VALID_CROW);
    }
    else
	validate_botline();

    // The lines of the cursor line itself are always used.
#ifdef FEAT_DIFF
    used = plines_nofill(cln);
#else
    validate_cheight();
    used = curwin->w_cline_height;
#endif

    // If the cursor is below botline, we will at least scroll by the height
    // of the cursor line.  Correct for empty lines, which are really part of
    // botline.
    if (cln >= curwin->w_botline)
    {
	scrolled = used;
	if (cln == curwin->w_botline)
	    scrolled -= curwin->w_empty_rows;
    }

    /*
     * Stop counting lines to scroll when
     * - hitting start of the file
     * - scrolled nothing or at least 'sj' lines
     * - at least 'scrolloff' lines below the cursor
     * - lines between botline and cursor have been counted
     */
#ifdef FEAT_FOLDING
    if (!hasFolding(curwin->w_cursor.lnum, &loff.lnum, &boff.lnum))
#endif
    {
	loff.lnum = cln;
	boff.lnum = cln;
    }
#ifdef FEAT_DIFF
    loff.fill = 0;
    boff.fill = 0;
    fill_below_window = diff_check_fill(curwin, curwin->w_botline)
						      - curwin->w_filler_rows;
#endif

    while (loff.lnum > 1)
    {
	// Stop when scrolled nothing or at least "min_scroll", found "extra"
	// context for 'scrolloff' and counted all lines below the window.
	if ((((scrolled <= 0 || scrolled >= min_scroll)
		    && extra >= (mouse_dragging > 0 ? mouse_dragging - 1 : so))
		    || boff.lnum + 1 > curbuf->b_ml.ml_line_count)
		&& loff.lnum <= curwin->w_botline
#ifdef FEAT_DIFF
		&& (loff.lnum < curwin->w_botline
		    || loff.fill >= fill_below_window)
#endif
		)
	    break;

	// Add one line above
	topline_back(&loff);
	if (loff.height == MAXCOL)
	    used = MAXCOL;
	else
	    used += loff.height;
	if (used > curwin->w_height)
	    break;
	if (loff.lnum >= curwin->w_botline
#ifdef FEAT_DIFF
		&& (loff.lnum > curwin->w_botline
		    || loff.fill <= fill_below_window)
#endif
		)
	{
	    // Count screen lines that are below the window.
	    scrolled += loff.height;
	    if (loff.lnum == curwin->w_botline
#ifdef FEAT_DIFF
			    && loff.fill == 0
#endif
		    )
		scrolled -= curwin->w_empty_rows;
	}

	if (boff.lnum < curbuf->b_ml.ml_line_count)
	{
	    // Add one line below
	    botline_forw(&boff);
	    used += boff.height;
	    if (used > curwin->w_height)
		break;
	    if (extra < ( mouse_dragging > 0 ? mouse_dragging - 1 : so)
		    || scrolled < min_scroll)
	    {
		extra += boff.height;
		if (boff.lnum >= curwin->w_botline
#ifdef FEAT_DIFF
			|| (boff.lnum + 1 == curwin->w_botline
			    && boff.fill > curwin->w_filler_rows)
#endif
		   )
		{
		    // Count screen lines that are below the window.
		    scrolled += boff.height;
		    if (boff.lnum == curwin->w_botline
#ifdef FEAT_DIFF
			    && boff.fill == 0
#endif
			    )
			scrolled -= curwin->w_empty_rows;
		}
	    }
	}
    }

    // curwin->w_empty_rows is larger, no need to scroll
    if (scrolled <= 0)
	line_count = 0;
    // more than a screenfull, don't scroll but redraw
    else if (used > curwin->w_height)
	line_count = used;
    // scroll minimal number of lines
    else
    {
	line_count = 0;
#ifdef FEAT_DIFF
	boff.fill = curwin->w_topfill;
#endif
	boff.lnum = curwin->w_topline - 1;
	for (i = 0; i < scrolled && boff.lnum < curwin->w_botline; )
	{
	    botline_forw(&boff);
	    i += boff.height;
	    ++line_count;
	}
	if (i < scrolled)	// below curwin->w_botline, don't scroll
	    line_count = 9999;
    }

    /*
     * Scroll up if the cursor is off the bottom of the screen a bit.
     * Otherwise put it at 1/2 of the screen.
     */
    if (line_count >= curwin->w_height && line_count > min_scroll)
	scroll_cursor_halfway(FALSE);
    else
	scrollup(line_count, TRUE);

    /*
     * If topline didn't change we need to restore w_botline and w_empty_rows
     * (we changed them).
     * If topline did change, update_screen() will set botline.
     */
    if (curwin->w_topline == old_topline && set_topbot)
    {
	curwin->w_botline = old_botline;
	curwin->w_empty_rows = old_empty_rows;
	curwin->w_valid = old_valid;
    }
    curwin->w_valid |= VALID_TOPLINE;
}