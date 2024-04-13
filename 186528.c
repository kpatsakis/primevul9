current_block(
    oparg_T	*oap,
    long	count,
    int		include,	// TRUE == include white space
    int		what,		// '(', '{', etc.
    int		other)		// ')', '}', etc.
{
    pos_T	old_pos;
    pos_T	*pos = NULL;
    pos_T	start_pos;
    pos_T	*end_pos;
    pos_T	old_start, old_end;
    char_u	*save_cpo;
    int		sol = FALSE;		// '{' at start of line

    old_pos = curwin->w_cursor;
    old_end = curwin->w_cursor;		// remember where we started
    old_start = old_end;

    /*
     * If we start on '(', '{', ')', '}', etc., use the whole block inclusive.
     */
    if (!VIsual_active || EQUAL_POS(VIsual, curwin->w_cursor))
    {
	setpcmark();
	if (what == '{')		// ignore indent
	    while (inindent(1))
		if (inc_cursor() != 0)
		    break;
	if (gchar_cursor() == what)
	    // cursor on '(' or '{', move cursor just after it
	    ++curwin->w_cursor.col;
    }
    else if (LT_POS(VIsual, curwin->w_cursor))
    {
	old_start = VIsual;
	curwin->w_cursor = VIsual;	    // cursor at low end of Visual
    }
    else
	old_end = VIsual;

    /*
     * Search backwards for unclosed '(', '{', etc..
     * Put this position in start_pos.
     * Ignore quotes here.  Keep the "M" flag in 'cpo', as that is what the
     * user wants.
     */
    save_cpo = p_cpo;
    p_cpo = (char_u *)(vim_strchr(p_cpo, CPO_MATCHBSL) != NULL ? "%M" : "%");
    if ((pos = findmatch(NULL, what)) != NULL)
    {
	while (count-- > 0)
	{
	    if ((pos = findmatch(NULL, what)) == NULL)
		break;
	    curwin->w_cursor = *pos;
	    start_pos = *pos;   // the findmatch for end_pos will overwrite *pos
	}
    }
    else
    {
	while (count-- > 0)
	{
	    if ((pos = findmatchlimit(NULL, what, FM_FORWARD, 0)) == NULL)
		break;
	    curwin->w_cursor = *pos;
	    start_pos = *pos;   // the findmatch for end_pos will overwrite *pos
	}
    }
    p_cpo = save_cpo;

    /*
     * Search for matching ')', '}', etc.
     * Put this position in curwin->w_cursor.
     */
    if (pos == NULL || (end_pos = findmatch(NULL, other)) == NULL)
    {
	curwin->w_cursor = old_pos;
	return FAIL;
    }
    curwin->w_cursor = *end_pos;

    /*
     * Try to exclude the '(', '{', ')', '}', etc. when "include" is FALSE.
     * If the ending '}', ')' or ']' is only preceded by indent, skip that
     * indent.  But only if the resulting area is not smaller than what we
     * started with.
     */
    while (!include)
    {
	incl(&start_pos);
	sol = (curwin->w_cursor.col == 0);
	decl(&curwin->w_cursor);
	while (inindent(1))
	{
	    sol = TRUE;
	    if (decl(&curwin->w_cursor) != 0)
		break;
	}

	/*
	 * In Visual mode, when the resulting area is not bigger than what we
	 * started with, extend it to the next block, and then exclude again.
	 * Don't try to expand the area if the area is empty.
	 */
	if (!LT_POS(start_pos, old_start) && !LT_POS(old_end, curwin->w_cursor)
		&& !EQUAL_POS(start_pos, curwin->w_cursor)
		&& VIsual_active)
	{
	    curwin->w_cursor = old_start;
	    decl(&curwin->w_cursor);
	    if ((pos = findmatch(NULL, what)) == NULL)
	    {
		curwin->w_cursor = old_pos;
		return FAIL;
	    }
	    start_pos = *pos;
	    curwin->w_cursor = *pos;
	    if ((end_pos = findmatch(NULL, other)) == NULL)
	    {
		curwin->w_cursor = old_pos;
		return FAIL;
	    }
	    curwin->w_cursor = *end_pos;
	}
	else
	    break;
    }

    if (VIsual_active)
    {
	if (*p_sel == 'e')
	    inc(&curwin->w_cursor);
	if (sol && gchar_cursor() != NUL)
	    inc(&curwin->w_cursor);	// include the line break
	VIsual = start_pos;
	VIsual_mode = 'v';
	redraw_curbuf_later(INVERTED);	// update the inversion
	showmode();
    }
    else
    {
	oap->start = start_pos;
	oap->motion_type = MCHAR;
	oap->inclusive = FALSE;
	if (sol)
	    incl(&curwin->w_cursor);
	else if (LTOREQ_POS(start_pos, curwin->w_cursor))
	    // Include the character under the cursor.
	    oap->inclusive = TRUE;
	else
	    // End is before the start (no text in between <>, [], etc.): don't
	    // operate on any text.
	    curwin->w_cursor = start_pos;
    }

    return OK;
}