current_word(
    oparg_T	*oap,
    long	count,
    int		include,	// TRUE: include word and white space
    int		bigword)	// FALSE == word, TRUE == WORD
{
    pos_T	start_pos;
    pos_T	pos;
    int		inclusive = TRUE;
    int		include_white = FALSE;

    cls_bigword = bigword;
    CLEAR_POS(&start_pos);

    // Correct cursor when 'selection' is exclusive
    if (VIsual_active && *p_sel == 'e' && LT_POS(VIsual, curwin->w_cursor))
	dec_cursor();

    /*
     * When Visual mode is not active, or when the VIsual area is only one
     * character, select the word and/or white space under the cursor.
     */
    if (!VIsual_active || EQUAL_POS(curwin->w_cursor, VIsual))
    {
	/*
	 * Go to start of current word or white space.
	 */
	back_in_line();
	start_pos = curwin->w_cursor;

	/*
	 * If the start is on white space, and white space should be included
	 * ("	word"), or start is not on white space, and white space should
	 * not be included ("word"), find end of word.
	 */
	if ((cls() == 0) == include)
	{
	    if (end_word(1L, bigword, TRUE, TRUE) == FAIL)
		return FAIL;
	}
	else
	{
	    /*
	     * If the start is not on white space, and white space should be
	     * included ("word	 "), or start is on white space and white
	     * space should not be included ("	 "), find start of word.
	     * If we end up in the first column of the next line (single char
	     * word) back up to end of the line.
	     */
	    fwd_word(1L, bigword, TRUE);
	    if (curwin->w_cursor.col == 0)
		decl(&curwin->w_cursor);
	    else
		oneleft();

	    if (include)
		include_white = TRUE;
	}

	if (VIsual_active)
	{
	    // should do something when inclusive == FALSE !
	    VIsual = start_pos;
	    redraw_curbuf_later(INVERTED);	// update the inversion
	}
	else
	{
	    oap->start = start_pos;
	    oap->motion_type = MCHAR;
	}
	--count;
    }

    /*
     * When count is still > 0, extend with more objects.
     */
    while (count > 0)
    {
	inclusive = TRUE;
	if (VIsual_active && LT_POS(curwin->w_cursor, VIsual))
	{
	    /*
	     * In Visual mode, with cursor at start: move cursor back.
	     */
	    if (decl(&curwin->w_cursor) == -1)
		return FAIL;
	    if (include != (cls() != 0))
	    {
		if (bck_word(1L, bigword, TRUE) == FAIL)
		    return FAIL;
	    }
	    else
	    {
		if (bckend_word(1L, bigword, TRUE) == FAIL)
		    return FAIL;
		(void)incl(&curwin->w_cursor);
	    }
	}
	else
	{
	    /*
	     * Move cursor forward one word and/or white area.
	     */
	    if (incl(&curwin->w_cursor) == -1)
		return FAIL;
	    if (include != (cls() == 0))
	    {
		if (fwd_word(1L, bigword, TRUE) == FAIL && count > 1)
		    return FAIL;
		/*
		 * If end is just past a new-line, we don't want to include
		 * the first character on the line.
		 * Put cursor on last char of white.
		 */
		if (oneleft() == FAIL)
		    inclusive = FALSE;
	    }
	    else
	    {
		if (end_word(1L, bigword, TRUE, TRUE) == FAIL)
		    return FAIL;
	    }
	}
	--count;
    }

    if (include_white && (cls() != 0
		 || (curwin->w_cursor.col == 0 && !inclusive)))
    {
	/*
	 * If we don't include white space at the end, move the start
	 * to include some white space there. This makes "daw" work
	 * better on the last word in a sentence (and "2daw" on last-but-one
	 * word).  Also when "2daw" deletes "word." at the end of the line
	 * (cursor is at start of next line).
	 * But don't delete white space at start of line (indent).
	 */
	pos = curwin->w_cursor;	// save cursor position
	curwin->w_cursor = start_pos;
	if (oneleft() == OK)
	{
	    back_in_line();
	    if (cls() == 0 && curwin->w_cursor.col > 0)
	    {
		if (VIsual_active)
		    VIsual = curwin->w_cursor;
		else
		    oap->start = curwin->w_cursor;
	    }
	}
	curwin->w_cursor = pos;	// put cursor back at end
    }

    if (VIsual_active)
    {
	if (*p_sel == 'e' && inclusive && LTOREQ_POS(VIsual, curwin->w_cursor))
	    inc_cursor();
	if (VIsual_mode == 'V')
	{
	    VIsual_mode = 'v';
	    redraw_cmdline = TRUE;		// show mode later
	}
    }
    else
	oap->inclusive = inclusive;

    return OK;
}