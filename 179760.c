cin_iswhileofdo_end(int terminated)
{
    char_u	*line;
    char_u	*p;
    char_u	*s;
    pos_T	*trypos;
    int		i;

    if (terminated != ';')	// there must be a ';' at the end
	return FALSE;

    p = line = ml_get_curline();
    while (*p != NUL)
    {
	p = cin_skipcomment(p);
	if (*p == ')')
	{
	    s = skipwhite(p + 1);
	    if (*s == ';' && cin_nocode(s + 1))
	    {
		// Found ");" at end of the line, now check there is "while"
		// before the matching '('.  XXX
		i = (int)(p - line);
		curwin->w_cursor.col = i;
		trypos = find_match_paren(curbuf->b_ind_maxparen);
		if (trypos != NULL)
		{
		    s = cin_skipcomment(ml_get(trypos->lnum));
		    if (*s == '}')		// accept "} while (cond);"
			s = cin_skipcomment(s + 1);
		    if (cin_starts_with(s, "while"))
		    {
			curwin->w_cursor.lnum = trypos->lnum;
			return TRUE;
		    }
		}

		// Searching may have made "line" invalid, get it again.
		line = ml_get_curline();
		p = line + i;
	    }
	}
	if (*p != NUL)
	    ++p;
    }
    return FALSE;
}