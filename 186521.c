current_tagblock(
    oparg_T	*oap,
    long	count_arg,
    int		include)	// TRUE == include white space
{
    long	count = count_arg;
    long	n;
    pos_T	old_pos;
    pos_T	start_pos;
    pos_T	end_pos;
    pos_T	old_start, old_end;
    char_u	*spat, *epat;
    char_u	*p;
    char_u	*cp;
    int		len;
    int		r;
    int		do_include = include;
    int		save_p_ws = p_ws;
    int		retval = FAIL;
    int		is_inclusive = TRUE;

    p_ws = FALSE;

    old_pos = curwin->w_cursor;
    old_end = curwin->w_cursor;		    // remember where we started
    old_start = old_end;
    if (!VIsual_active || *p_sel == 'e')
	decl(&old_end);			    // old_end is inclusive

    /*
     * If we start on "<aaa>" select that block.
     */
    if (!VIsual_active || EQUAL_POS(VIsual, curwin->w_cursor))
    {
	setpcmark();

	// ignore indent
	while (inindent(1))
	    if (inc_cursor() != 0)
		break;

	if (in_html_tag(FALSE))
	{
	    // cursor on start tag, move to its '>'
	    while (*ml_get_cursor() != '>')
		if (inc_cursor() < 0)
		    break;
	}
	else if (in_html_tag(TRUE))
	{
	    // cursor on end tag, move to just before it
	    while (*ml_get_cursor() != '<')
		if (dec_cursor() < 0)
		    break;
	    dec_cursor();
	    old_end = curwin->w_cursor;
	}
    }
    else if (LT_POS(VIsual, curwin->w_cursor))
    {
	old_start = VIsual;
	curwin->w_cursor = VIsual;	    // cursor at low end of Visual
    }
    else
	old_end = VIsual;

again:
    /*
     * Search backwards for unclosed "<aaa>".
     * Put this position in start_pos.
     */
    for (n = 0; n < count; ++n)
    {
	if (do_searchpair((char_u *)"<[^ \t>/!]\\+\\%(\\_s\\_[^>]\\{-}[^/]>\\|$\\|\\_s\\=>\\)",
		    (char_u *)"",
		    (char_u *)"</[^>]*>", BACKWARD, NULL, 0,
						  NULL, (linenr_T)0, 0L) <= 0)
	{
	    curwin->w_cursor = old_pos;
	    goto theend;
	}
    }
    start_pos = curwin->w_cursor;

    /*
     * Search for matching "</aaa>".  First isolate the "aaa".
     */
    inc_cursor();
    p = ml_get_cursor();
    for (cp = p; *cp != NUL && *cp != '>' && !VIM_ISWHITE(*cp); MB_PTR_ADV(cp))
	;
    len = (int)(cp - p);
    if (len == 0)
    {
	curwin->w_cursor = old_pos;
	goto theend;
    }
    spat = alloc(len + 39);
    epat = alloc(len + 9);
    if (spat == NULL || epat == NULL)
    {
	vim_free(spat);
	vim_free(epat);
	curwin->w_cursor = old_pos;
	goto theend;
    }
    sprintf((char *)spat, "<%.*s\\>\\%%(\\_s\\_[^>]\\{-}\\_[^/]>\\|\\_s\\?>\\)\\c", len, p);
    sprintf((char *)epat, "</%.*s>\\c", len, p);

    r = do_searchpair(spat, (char_u *)"", epat, FORWARD, NULL,
						    0, NULL, (linenr_T)0, 0L);

    vim_free(spat);
    vim_free(epat);

    if (r < 1 || LT_POS(curwin->w_cursor, old_end))
    {
	// Can't find other end or it's before the previous end.  Could be a
	// HTML tag that doesn't have a matching end.  Search backwards for
	// another starting tag.
	count = 1;
	curwin->w_cursor = start_pos;
	goto again;
    }

    if (do_include)
    {
	// Include up to the '>'.
	while (*ml_get_cursor() != '>')
	    if (inc_cursor() < 0)
		break;
    }
    else
    {
	char_u *c = ml_get_cursor();

	// Exclude the '<' of the end tag.
	// If the closing tag is on new line, do not decrement cursor, but
	// make operation exclusive, so that the linefeed will be selected
	if (*c == '<' && !VIsual_active && curwin->w_cursor.col == 0)
	    // do not decrement cursor
	    is_inclusive = FALSE;
	else if (*c == '<')
	    dec_cursor();
    }
    end_pos = curwin->w_cursor;

    if (!do_include)
    {
	// Exclude the start tag.
	curwin->w_cursor = start_pos;
	while (inc_cursor() >= 0)
	    if (*ml_get_cursor() == '>')
	    {
		inc_cursor();
		start_pos = curwin->w_cursor;
		break;
	    }
	curwin->w_cursor = end_pos;

	// If we are in Visual mode and now have the same text as before set
	// "do_include" and try again.
	if (VIsual_active && EQUAL_POS(start_pos, old_start)
						&& EQUAL_POS(end_pos, old_end))
	{
	    do_include = TRUE;
	    curwin->w_cursor = old_start;
	    count = count_arg;
	    goto again;
	}
    }

    if (VIsual_active)
    {
	// If the end is before the start there is no text between tags, select
	// the char under the cursor.
	if (LT_POS(end_pos, start_pos))
	    curwin->w_cursor = start_pos;
	else if (*p_sel == 'e')
	    inc_cursor();
	VIsual = start_pos;
	VIsual_mode = 'v';
	redraw_curbuf_later(INVERTED);	// update the inversion
	showmode();
    }
    else
    {
	oap->start = start_pos;
	oap->motion_type = MCHAR;
	if (LT_POS(end_pos, start_pos))
	{
	    // End is before the start: there is no text between tags; operate
	    // on an empty area.
	    curwin->w_cursor = start_pos;
	    oap->inclusive = FALSE;
	}
	else
	    oap->inclusive = is_inclusive;
    }
    retval = OK;

theend:
    p_ws = save_p_ws;
    return retval;
}