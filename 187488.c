auto_format(
    int		trailblank,	// when TRUE also format with trailing blank
    int		prev_line)	// may start in previous line
{
    pos_T	pos;
    colnr_T	len;
    char_u	*old;
    char_u	*new, *pnew;
    int		wasatend;
    int		cc;

    if (!has_format_option(FO_AUTO))
	return;

    pos = curwin->w_cursor;
    old = ml_get_curline();

    // may remove added space
    check_auto_format(FALSE);

    // Don't format in Insert mode when the cursor is on a trailing blank, the
    // user might insert normal text next.  Also skip formatting when "1" is
    // in 'formatoptions' and there is a single character before the cursor.
    // Otherwise the line would be broken and when typing another non-white
    // next they are not joined back together.
    wasatend = (pos.col == (colnr_T)STRLEN(old));
    if (*old != NUL && !trailblank && wasatend)
    {
	dec_cursor();
	cc = gchar_cursor();
	if (!WHITECHAR(cc) && curwin->w_cursor.col > 0
					  && has_format_option(FO_ONE_LETTER))
	    dec_cursor();
	cc = gchar_cursor();
	if (WHITECHAR(cc))
	{
	    curwin->w_cursor = pos;
	    return;
	}
	curwin->w_cursor = pos;
    }

    // With the 'c' flag in 'formatoptions' and 't' missing: only format
    // comments.
    if (has_format_option(FO_WRAP_COMS) && !has_format_option(FO_WRAP)
				&& get_leader_len(old, NULL, FALSE, TRUE) == 0)
	return;

    // May start formatting in a previous line, so that after "x" a word is
    // moved to the previous line if it fits there now.  Only when this is not
    // the start of a paragraph.
    if (prev_line && !paragraph_start(curwin->w_cursor.lnum))
    {
	--curwin->w_cursor.lnum;
	if (u_save_cursor() == FAIL)
	    return;
    }

    // Do the formatting and restore the cursor position.  "saved_cursor" will
    // be adjusted for the text formatting.
    saved_cursor = pos;
    format_lines((linenr_T)-1, FALSE);
    curwin->w_cursor = saved_cursor;
    saved_cursor.lnum = 0;

    if (curwin->w_cursor.lnum > curbuf->b_ml.ml_line_count)
    {
	// "cannot happen"
	curwin->w_cursor.lnum = curbuf->b_ml.ml_line_count;
	coladvance((colnr_T)MAXCOL);
    }
    else
	check_cursor_col();

    // Insert mode: If the cursor is now after the end of the line while it
    // previously wasn't, the line was broken.  Because of the rule above we
    // need to add a space when 'w' is in 'formatoptions' to keep a paragraph
    // formatted.
    if (!wasatend && has_format_option(FO_WHITE_PAR))
    {
	new = ml_get_curline();
	len = (colnr_T)STRLEN(new);
	if (curwin->w_cursor.col == len)
	{
	    pnew = vim_strnsave(new, len + 2);
	    pnew[len] = ' ';
	    pnew[len + 1] = NUL;
	    ml_replace(curwin->w_cursor.lnum, pnew, FALSE);
	    // remove the space later
	    did_add_space = TRUE;
	}
	else
	    // may remove added space
	    check_auto_format(FALSE);
    }

    check_cursor();
}