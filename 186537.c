fwd_word(
    long	count,
    int		bigword,    // "W", "E" or "B"
    int		eol)
{
    int		sclass;	    // starting class
    int		i;
    int		last_line;

    curwin->w_cursor.coladd = 0;
    cls_bigword = bigword;
    while (--count >= 0)
    {
#ifdef FEAT_FOLDING
	// When inside a range of folded lines, move to the last char of the
	// last line.
	if (hasFolding(curwin->w_cursor.lnum, NULL, &curwin->w_cursor.lnum))
	    coladvance((colnr_T)MAXCOL);
#endif
	sclass = cls();

	/*
	 * We always move at least one character, unless on the last
	 * character in the buffer.
	 */
	last_line = (curwin->w_cursor.lnum == curbuf->b_ml.ml_line_count);
	i = inc_cursor();
	if (i == -1 || (i >= 1 && last_line)) // started at last char in file
	    return FAIL;
	if (i >= 1 && eol && count == 0)      // started at last char in line
	    return OK;

	/*
	 * Go one char past end of current word (if any)
	 */
	if (sclass != 0)
	    while (cls() == sclass)
	    {
		i = inc_cursor();
		if (i == -1 || (i >= 1 && eol && count == 0))
		    return OK;
	    }

	/*
	 * go to next non-white
	 */
	while (cls() == 0)
	{
	    /*
	     * We'll stop if we land on a blank line
	     */
	    if (curwin->w_cursor.col == 0 && *ml_get_curline() == NUL)
		break;

	    i = inc_cursor();
	    if (i == -1 || (i >= 1 && eol && count == 0))
		return OK;
	}
    }
    return OK;
}