cin_islabel(void)		// XXX
{
    char_u	*s;

    s = cin_skipcomment(ml_get_curline());

    // Exclude "default" from labels, since it should be indented
    // like a switch label.  Same for C++ scope declarations.
    if (cin_isdefault(s))
	return FALSE;
    if (cin_isscopedecl(s))
	return FALSE;

    if (cin_islabel_skip(&s))
    {
	// Only accept a label if the previous line is terminated or is a case
	// label.
	pos_T	cursor_save;
	pos_T	*trypos;
	char_u	*line;

	cursor_save = curwin->w_cursor;
	while (curwin->w_cursor.lnum > 1)
	{
	    --curwin->w_cursor.lnum;

	    // If we're in a comment or raw string now, skip to the start of
	    // it.
	    curwin->w_cursor.col = 0;
	    if ((trypos = ind_find_start_CORS(NULL)) != NULL) // XXX
		curwin->w_cursor = *trypos;

	    line = ml_get_curline();
	    if (cin_ispreproc(line))	// ignore #defines, #if, etc.
		continue;
	    if (*(line = cin_skipcomment(line)) == NUL)
		continue;

	    curwin->w_cursor = cursor_save;
	    if (cin_isterminated(line, TRUE, FALSE)
		    || cin_isscopedecl(line)
		    || cin_iscase(line, TRUE)
		    || (cin_islabel_skip(&line) && cin_nocode(line)))
		return TRUE;
	    return FALSE;
	}
	curwin->w_cursor = cursor_save;
	return TRUE;		// label at start of file???
    }
    return FALSE;
}