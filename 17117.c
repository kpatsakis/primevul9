onepage(int dir, long count)
{
    long	n;
    int		retval = OK;
    lineoff_T	loff;
    linenr_T	old_topline = curwin->w_topline;
    long        so = get_scrolloff_value();

    if (curbuf->b_ml.ml_line_count == 1)    // nothing to do
    {
	beep_flush();
	return FAIL;
    }

    for ( ; count > 0; --count)
    {
	validate_botline();
	/*
	 * It's an error to move a page up when the first line is already on
	 * the screen.	It's an error to move a page down when the last line
	 * is on the screen and the topline is 'scrolloff' lines from the
	 * last line.
	 */
	if (dir == FORWARD
		? ((curwin->w_topline >= curbuf->b_ml.ml_line_count - so)
		    && curwin->w_botline > curbuf->b_ml.ml_line_count)
		: (curwin->w_topline == 1
#ifdef FEAT_DIFF
		    && curwin->w_topfill ==
				    diff_check_fill(curwin, curwin->w_topline)
#endif
		    ))
	{
	    beep_flush();
	    retval = FAIL;
	    break;
	}

#ifdef FEAT_DIFF
	loff.fill = 0;
#endif
	if (dir == FORWARD)
	{
	    if (ONE_WINDOW && p_window > 0 && p_window < Rows - 1)
	    {
		// Vi compatible scrolling
		if (p_window <= 2)
		    ++curwin->w_topline;
		else
		    curwin->w_topline += p_window - 2;
		if (curwin->w_topline > curbuf->b_ml.ml_line_count)
		    curwin->w_topline = curbuf->b_ml.ml_line_count;
		curwin->w_cursor.lnum = curwin->w_topline;
	    }
	    else if (curwin->w_botline > curbuf->b_ml.ml_line_count)
	    {
		// at end of file
		curwin->w_topline = curbuf->b_ml.ml_line_count;
#ifdef FEAT_DIFF
		curwin->w_topfill = 0;
#endif
		curwin->w_valid &= ~(VALID_WROW|VALID_CROW);
	    }
	    else
	    {
		// For the overlap, start with the line just below the window
		// and go upwards.
		loff.lnum = curwin->w_botline;
#ifdef FEAT_DIFF
		loff.fill = diff_check_fill(curwin, loff.lnum)
						      - curwin->w_filler_rows;
#endif
		get_scroll_overlap(&loff, -1);
		curwin->w_topline = loff.lnum;
#ifdef FEAT_DIFF
		curwin->w_topfill = loff.fill;
		check_topfill(curwin, FALSE);
#endif
		curwin->w_cursor.lnum = curwin->w_topline;
		curwin->w_valid &= ~(VALID_WCOL|VALID_CHEIGHT|VALID_WROW|
				   VALID_CROW|VALID_BOTLINE|VALID_BOTLINE_AP);
	    }
	}
	else	// dir == BACKWARDS
	{
#ifdef FEAT_DIFF
	    if (curwin->w_topline == 1)
	    {
		// Include max number of filler lines
		max_topfill();
		continue;
	    }
#endif
	    if (ONE_WINDOW && p_window > 0 && p_window < Rows - 1)
	    {
		// Vi compatible scrolling (sort of)
		if (p_window <= 2)
		    --curwin->w_topline;
		else
		    curwin->w_topline -= p_window - 2;
		if (curwin->w_topline < 1)
		    curwin->w_topline = 1;
		curwin->w_cursor.lnum = curwin->w_topline + p_window - 1;
		if (curwin->w_cursor.lnum > curbuf->b_ml.ml_line_count)
		    curwin->w_cursor.lnum = curbuf->b_ml.ml_line_count;
		continue;
	    }

	    // Find the line at the top of the window that is going to be the
	    // line at the bottom of the window.  Make sure this results in
	    // the same line as before doing CTRL-F.
	    loff.lnum = curwin->w_topline - 1;
#ifdef FEAT_DIFF
	    loff.fill = diff_check_fill(curwin, loff.lnum + 1)
							  - curwin->w_topfill;
#endif
	    get_scroll_overlap(&loff, 1);

	    if (loff.lnum >= curbuf->b_ml.ml_line_count)
	    {
		loff.lnum = curbuf->b_ml.ml_line_count;
#ifdef FEAT_DIFF
		loff.fill = 0;
	    }
	    else
	    {
		botline_topline(&loff);
#endif
	    }
	    curwin->w_cursor.lnum = loff.lnum;

	    // Find the line just above the new topline to get the right line
	    // at the bottom of the window.
	    n = 0;
	    while (n <= curwin->w_height && loff.lnum >= 1)
	    {
		topline_back(&loff);
		if (loff.height == MAXCOL)
		    n = MAXCOL;
		else
		    n += loff.height;
	    }
	    if (loff.lnum < 1)			// at begin of file
	    {
		curwin->w_topline = 1;
#ifdef FEAT_DIFF
		max_topfill();
#endif
		curwin->w_valid &= ~(VALID_WROW|VALID_CROW|VALID_BOTLINE);
	    }
	    else
	    {
		// Go two lines forward again.
#ifdef FEAT_DIFF
		topline_botline(&loff);
#endif
		botline_forw(&loff);
		botline_forw(&loff);
#ifdef FEAT_DIFF
		botline_topline(&loff);
#endif
#ifdef FEAT_FOLDING
		// We're at the wrong end of a fold now.
		(void)hasFolding(loff.lnum, &loff.lnum, NULL);
#endif

		// Always scroll at least one line.  Avoid getting stuck on
		// very long lines.
		if (loff.lnum >= curwin->w_topline
#ifdef FEAT_DIFF
			&& (loff.lnum > curwin->w_topline
			    || loff.fill >= curwin->w_topfill)
#endif
			)
		{
#ifdef FEAT_DIFF
		    // First try using the maximum number of filler lines.  If
		    // that's not enough, backup one line.
		    loff.fill = curwin->w_topfill;
		    if (curwin->w_topfill < diff_check_fill(curwin,
							   curwin->w_topline))
			max_topfill();
		    if (curwin->w_topfill == loff.fill)
#endif
		    {
			--curwin->w_topline;
#ifdef FEAT_DIFF
			curwin->w_topfill = 0;
#endif
		    }
		    comp_botline(curwin);
		    curwin->w_cursor.lnum = curwin->w_botline - 1;
		    curwin->w_valid &=
			    ~(VALID_WCOL|VALID_CHEIGHT|VALID_WROW|VALID_CROW);
		}
		else
		{
		    curwin->w_topline = loff.lnum;
#ifdef FEAT_DIFF
		    curwin->w_topfill = loff.fill;
		    check_topfill(curwin, FALSE);
#endif
		    curwin->w_valid &= ~(VALID_WROW|VALID_CROW|VALID_BOTLINE);
		}
	    }
	}
    }
#ifdef FEAT_FOLDING
    foldAdjustCursor();
#endif
    cursor_correct();
    check_cursor_col();
    if (retval == OK)
	beginline(BL_SOL | BL_FIX);
    curwin->w_valid &= ~(VALID_WCOL|VALID_WROW|VALID_VIRTCOL);

    if (retval == OK && dir == FORWARD)
    {
	// Avoid the screen jumping up and down when 'scrolloff' is non-zero.
	// But make sure we scroll at least one line (happens with mix of long
	// wrapping lines and non-wrapping line).
	if (check_top_offset())
	{
	    scroll_cursor_top(1, FALSE);
	    if (curwin->w_topline <= old_topline
				  && old_topline < curbuf->b_ml.ml_line_count)
	    {
		curwin->w_topline = old_topline + 1;
#ifdef FEAT_FOLDING
		(void)hasFolding(curwin->w_topline, &curwin->w_topline, NULL);
#endif
	    }
	}
#ifdef FEAT_FOLDING
	else if (curwin->w_botline > curbuf->b_ml.ml_line_count)
	    (void)hasFolding(curwin->w_topline, &curwin->w_topline, NULL);
#endif
    }

    redraw_later(VALID);
    return retval;
}