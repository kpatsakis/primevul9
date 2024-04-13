cin_iswhileofdo (char_u *p, linenr_T lnum)	// XXX
{
    pos_T	cursor_save;
    pos_T	*trypos;
    int		retval = FALSE;

    p = cin_skipcomment(p);
    if (*p == '}')		// accept "} while (cond);"
	p = cin_skipcomment(p + 1);
    if (cin_starts_with(p, "while"))
    {
	cursor_save = curwin->w_cursor;
	curwin->w_cursor.lnum = lnum;
	curwin->w_cursor.col = 0;
	p = ml_get_curline();
	while (*p && *p != 'w')	// skip any '}', until the 'w' of the "while"
	{
	    ++p;
	    ++curwin->w_cursor.col;
	}
	if ((trypos = findmatchlimit(NULL, 0, 0,
					      curbuf->b_ind_maxparen)) != NULL
		&& *cin_skipcomment(ml_get_pos(trypos) + 1) == ';')
	    retval = TRUE;
	curwin->w_cursor = cursor_save;
    }
    return retval;
}