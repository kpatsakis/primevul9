halfpage(int flag, linenr_T Prenum)
{
    long	scrolled = 0;
    int		i;
    int		n;
    int		room;

    if (Prenum)
	curwin->w_p_scr = (Prenum > curwin->w_height) ?
						curwin->w_height : Prenum;
    n = (curwin->w_p_scr <= curwin->w_height) ?
				    curwin->w_p_scr : curwin->w_height;

    update_topline();
    validate_botline();
    room = curwin->w_empty_rows;
#ifdef FEAT_DIFF
    room += curwin->w_filler_rows;
#endif
    if (flag)
    {
	/*
	 * scroll the text up
	 */
	while (n > 0 && curwin->w_botline <= curbuf->b_ml.ml_line_count)
	{
#ifdef FEAT_DIFF
	    if (curwin->w_topfill > 0)
	    {
		i = 1;
		--n;
		--curwin->w_topfill;
	    }
	    else
#endif
	    {
		i = PLINES_NOFILL(curwin->w_topline);
		n -= i;
		if (n < 0 && scrolled > 0)
		    break;
#ifdef FEAT_FOLDING
		(void)hasFolding(curwin->w_topline, NULL, &curwin->w_topline);
#endif
		++curwin->w_topline;
#ifdef FEAT_DIFF
		curwin->w_topfill = diff_check_fill(curwin, curwin->w_topline);
#endif

		if (curwin->w_cursor.lnum < curbuf->b_ml.ml_line_count)
		{
		    ++curwin->w_cursor.lnum;
		    curwin->w_valid &=
				    ~(VALID_VIRTCOL|VALID_CHEIGHT|VALID_WCOL);
		}
	    }
	    curwin->w_valid &= ~(VALID_CROW|VALID_WROW);
	    scrolled += i;

	    /*
	     * Correct w_botline for changed w_topline.
	     * Won't work when there are filler lines.
	     */
#ifdef FEAT_DIFF
	    if (curwin->w_p_diff)
		curwin->w_valid &= ~(VALID_BOTLINE|VALID_BOTLINE_AP);
	    else
#endif
	    {
		room += i;
		do
		{
		    i = plines(curwin->w_botline);
		    if (i > room)
			break;
#ifdef FEAT_FOLDING
		    (void)hasFolding(curwin->w_botline, NULL,
							  &curwin->w_botline);
#endif
		    ++curwin->w_botline;
		    room -= i;
		} while (curwin->w_botline <= curbuf->b_ml.ml_line_count);
	    }
	}

	/*
	 * When hit bottom of the file: move cursor down.
	 */
	if (n > 0)
	{
# ifdef FEAT_FOLDING
	    if (hasAnyFolding(curwin))
	    {
		while (--n >= 0
			&& curwin->w_cursor.lnum < curbuf->b_ml.ml_line_count)
		{
		    (void)hasFolding(curwin->w_cursor.lnum, NULL,
						      &curwin->w_cursor.lnum);
		    ++curwin->w_cursor.lnum;
		}
	    }
	    else
# endif
		curwin->w_cursor.lnum += n;
	    check_cursor_lnum();
	}
    }
    else
    {
	/*
	 * scroll the text down
	 */
	while (n > 0 && curwin->w_topline > 1)
	{
#ifdef FEAT_DIFF
	    if (curwin->w_topfill < diff_check_fill(curwin, curwin->w_topline))
	    {
		i = 1;
		--n;
		++curwin->w_topfill;
	    }
	    else
#endif
	    {
		i = PLINES_NOFILL(curwin->w_topline - 1);
		n -= i;
		if (n < 0 && scrolled > 0)
		    break;
		--curwin->w_topline;
#ifdef FEAT_FOLDING
		(void)hasFolding(curwin->w_topline, &curwin->w_topline, NULL);
#endif
#ifdef FEAT_DIFF
		curwin->w_topfill = 0;
#endif
	    }
	    curwin->w_valid &= ~(VALID_CROW|VALID_WROW|
					      VALID_BOTLINE|VALID_BOTLINE_AP);
	    scrolled += i;
	    if (curwin->w_cursor.lnum > 1)
	    {
		--curwin->w_cursor.lnum;
		curwin->w_valid &= ~(VALID_VIRTCOL|VALID_CHEIGHT|VALID_WCOL);
	    }
	}

	/*
	 * When hit top of the file: move cursor up.
	 */
	if (n > 0)
	{
	    if (curwin->w_cursor.lnum <= (linenr_T)n)
		curwin->w_cursor.lnum = 1;
	    else
# ifdef FEAT_FOLDING
	    if (hasAnyFolding(curwin))
	    {
		while (--n >= 0 && curwin->w_cursor.lnum > 1)
		{
		    --curwin->w_cursor.lnum;
		    (void)hasFolding(curwin->w_cursor.lnum,
						&curwin->w_cursor.lnum, NULL);
		}
	    }
	    else
# endif
		curwin->w_cursor.lnum -= n;
	}
    }
# ifdef FEAT_FOLDING
    // Move cursor to first line of closed fold.
    foldAdjustCursor();
# endif
#ifdef FEAT_DIFF
    check_topfill(curwin, !flag);
#endif
    cursor_correct();
    beginline(BL_SOL | BL_FIX);
    redraw_later(VALID);
}