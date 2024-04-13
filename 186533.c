current_sent(oparg_T *oap, long count, int include)
{
    pos_T	start_pos;
    pos_T	pos;
    int		start_blank;
    int		c;
    int		at_start_sent;
    long	ncount;

    start_pos = curwin->w_cursor;
    pos = start_pos;
    findsent(FORWARD, 1L);	// Find start of next sentence.

    /*
     * When the Visual area is bigger than one character: Extend it.
     */
    if (VIsual_active && !EQUAL_POS(start_pos, VIsual))
    {
extend:
	if (LT_POS(start_pos, VIsual))
	{
	    /*
	     * Cursor at start of Visual area.
	     * Find out where we are:
	     * - in the white space before a sentence
	     * - in a sentence or just after it
	     * - at the start of a sentence
	     */
	    at_start_sent = TRUE;
	    decl(&pos);
	    while (LT_POS(pos, curwin->w_cursor))
	    {
		c = gchar_pos(&pos);
		if (!VIM_ISWHITE(c))
		{
		    at_start_sent = FALSE;
		    break;
		}
		incl(&pos);
	    }
	    if (!at_start_sent)
	    {
		findsent(BACKWARD, 1L);
		if (EQUAL_POS(curwin->w_cursor, start_pos))
		    at_start_sent = TRUE;  // exactly at start of sentence
		else
		    // inside a sentence, go to its end (start of next)
		    findsent(FORWARD, 1L);
	    }
	    if (include)	// "as" gets twice as much as "is"
		count *= 2;
	    while (count--)
	    {
		if (at_start_sent)
		    find_first_blank(&curwin->w_cursor);
		c = gchar_cursor();
		if (!at_start_sent || (!include && !VIM_ISWHITE(c)))
		    findsent(BACKWARD, 1L);
		at_start_sent = !at_start_sent;
	    }
	}
	else
	{
	    /*
	     * Cursor at end of Visual area.
	     * Find out where we are:
	     * - just before a sentence
	     * - just before or in the white space before a sentence
	     * - in a sentence
	     */
	    incl(&pos);
	    at_start_sent = TRUE;
	    // not just before a sentence
	    if (!EQUAL_POS(pos, curwin->w_cursor))
	    {
		at_start_sent = FALSE;
		while (LT_POS(pos, curwin->w_cursor))
		{
		    c = gchar_pos(&pos);
		    if (!VIM_ISWHITE(c))
		    {
			at_start_sent = TRUE;
			break;
		    }
		    incl(&pos);
		}
		if (at_start_sent)	// in the sentence
		    findsent(BACKWARD, 1L);
		else		// in/before white before a sentence
		    curwin->w_cursor = start_pos;
	    }

	    if (include)	// "as" gets twice as much as "is"
		count *= 2;
	    findsent_forward(count, at_start_sent);
	    if (*p_sel == 'e')
		++curwin->w_cursor.col;
	}
	return OK;
    }

    /*
     * If the cursor started on a blank, check if it is just before the start
     * of the next sentence.
     */
    while (c = gchar_pos(&pos), VIM_ISWHITE(c))	// VIM_ISWHITE() is a macro
	incl(&pos);
    if (EQUAL_POS(pos, curwin->w_cursor))
    {
	start_blank = TRUE;
	find_first_blank(&start_pos);	// go back to first blank
    }
    else
    {
	start_blank = FALSE;
	findsent(BACKWARD, 1L);
	start_pos = curwin->w_cursor;
    }
    if (include)
	ncount = count * 2;
    else
    {
	ncount = count;
	if (start_blank)
	    --ncount;
    }
    if (ncount > 0)
	findsent_forward(ncount, TRUE);
    else
	decl(&curwin->w_cursor);

    if (include)
    {
	/*
	 * If the blank in front of the sentence is included, exclude the
	 * blanks at the end of the sentence, go back to the first blank.
	 * If there are no trailing blanks, try to include leading blanks.
	 */
	if (start_blank)
	{
	    find_first_blank(&curwin->w_cursor);
	    c = gchar_pos(&curwin->w_cursor);	// VIM_ISWHITE() is a macro
	    if (VIM_ISWHITE(c))
		decl(&curwin->w_cursor);
	}
	else if (c = gchar_cursor(), !VIM_ISWHITE(c))
	    find_first_blank(&start_pos);
    }

    if (VIsual_active)
    {
	// Avoid getting stuck with "is" on a single space before a sentence.
	if (EQUAL_POS(start_pos, curwin->w_cursor))
	    goto extend;
	if (*p_sel == 'e')
	    ++curwin->w_cursor.col;
	VIsual = start_pos;
	VIsual_mode = 'v';
	redraw_cmdline = TRUE;		// show mode later
	redraw_curbuf_later(INVERTED);	// update the inversion
    }
    else
    {
	// include a newline after the sentence, if there is one
	if (incl(&curwin->w_cursor) == -1)
	    oap->inclusive = TRUE;
	else
	    oap->inclusive = FALSE;
	oap->start = start_pos;
	oap->motion_type = MCHAR;
    }
    return OK;
}