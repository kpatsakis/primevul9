findsent(int dir, long count)
{
    pos_T	pos, tpos;
    pos_T	prev_pos;
    int		c;
    int		(*func)(pos_T *);
    int		startlnum;
    int		noskip = FALSE;	    // do not skip blanks
    int		cpo_J;
    int		found_dot;

    pos = curwin->w_cursor;
    if (dir == FORWARD)
	func = incl;
    else
	func = decl;

    while (count--)
    {
	prev_pos = pos;

	/*
	 * if on an empty line, skip up to a non-empty line
	 */
	if (gchar_pos(&pos) == NUL)
	{
	    do
		if ((*func)(&pos) == -1)
		    break;
	    while (gchar_pos(&pos) == NUL);
	    if (dir == FORWARD)
		goto found;
	}
	/*
	 * if on the start of a paragraph or a section and searching forward,
	 * go to the next line
	 */
	else if (dir == FORWARD && pos.col == 0 &&
						startPS(pos.lnum, NUL, FALSE))
	{
	    if (pos.lnum == curbuf->b_ml.ml_line_count)
		return FAIL;
	    ++pos.lnum;
	    goto found;
	}
	else if (dir == BACKWARD)
	    decl(&pos);

	// go back to the previous non-white non-punctuation character
	found_dot = FALSE;
	while (c = gchar_pos(&pos), VIM_ISWHITE(c)
				|| vim_strchr((char_u *)".!?)]\"'", c) != NULL)
	{
	    tpos = pos;
	    if (decl(&tpos) == -1 || (LINEEMPTY(tpos.lnum) && dir == FORWARD))
		break;

	    if (found_dot)
		break;
	    if (vim_strchr((char_u *) ".!?", c) != NULL)
		found_dot = TRUE;

	    if (vim_strchr((char_u *) ")]\"'", c) != NULL
		&& vim_strchr((char_u *) ".!?)]\"'", gchar_pos(&tpos)) == NULL)
		break;

	    decl(&pos);
	}

	// remember the line where the search started
	startlnum = pos.lnum;
	cpo_J = vim_strchr(p_cpo, CPO_ENDOFSENT) != NULL;

	for (;;)		// find end of sentence
	{
	    c = gchar_pos(&pos);
	    if (c == NUL || (pos.col == 0 && startPS(pos.lnum, NUL, FALSE)))
	    {
		if (dir == BACKWARD && pos.lnum != startlnum)
		    ++pos.lnum;
		break;
	    }
	    if (c == '.' || c == '!' || c == '?')
	    {
		tpos = pos;
		do
		    if ((c = inc(&tpos)) == -1)
			break;
		while (vim_strchr((char_u *)")]\"'", c = gchar_pos(&tpos))
			!= NULL);
		if (c == -1  || (!cpo_J && (c == ' ' || c == '\t')) || c == NUL
		    || (cpo_J && (c == ' ' && inc(&tpos) >= 0
			      && gchar_pos(&tpos) == ' ')))
		{
		    pos = tpos;
		    if (gchar_pos(&pos) == NUL) // skip NUL at EOL
			inc(&pos);
		    break;
		}
	    }
	    if ((*func)(&pos) == -1)
	    {
		if (count)
		    return FAIL;
		noskip = TRUE;
		break;
	    }
	}
found:
	    // skip white space
	while (!noskip && ((c = gchar_pos(&pos)) == ' ' || c == '\t'))
	    if (incl(&pos) == -1)
		break;

	if (EQUAL_POS(prev_pos, pos))
	{
	    // didn't actually move, advance one character and try again
	    if ((*func)(&pos) == -1)
	    {
		if (count)
		    return FAIL;
		break;
	    }
	    ++count;
	}
    }

    setpcmark();
    curwin->w_cursor = pos;
    return OK;
}