in_cinkeys(
    int		keytyped,
    int		when,
    int		line_is_empty)
{
    char_u	*look;
    int		try_match;
    int		try_match_word;
    char_u	*p;
    char_u	*line;
    int		icase;
    int		i;

    if (keytyped == NUL)
	// Can happen with CTRL-Y and CTRL-E on a short line.
	return FALSE;

#ifdef FEAT_EVAL
    if (*curbuf->b_p_inde != NUL)
	look = curbuf->b_p_indk;	// 'indentexpr' set: use 'indentkeys'
    else
#endif
	look = curbuf->b_p_cink;	// 'indentexpr' empty: use 'cinkeys'
    while (*look)
    {
	// Find out if we want to try a match with this key, depending on
	// 'when' and a '*' or '!' before the key.
	switch (when)
	{
	    case '*': try_match = (*look == '*'); break;
	    case '!': try_match = (*look == '!'); break;
	     default: try_match = (*look != '*'); break;
	}
	if (*look == '*' || *look == '!')
	    ++look;

	// If there is a '0', only accept a match if the line is empty.
	// But may still match when typing last char of a word.
	if (*look == '0')
	{
	    try_match_word = try_match;
	    if (!line_is_empty)
		try_match = FALSE;
	    ++look;
	}
	else
	    try_match_word = FALSE;

	// does it look like a control character?
	if (*look == '^' && look[1] >= '?' && look[1] <= '_')
	{
	    if (try_match && keytyped == Ctrl_chr(look[1]))
		return TRUE;
	    look += 2;
	}
	// 'o' means "o" command, open forward.
	// 'O' means "O" command, open backward.
	else if (*look == 'o')
	{
	    if (try_match && keytyped == KEY_OPEN_FORW)
		return TRUE;
	    ++look;
	}
	else if (*look == 'O')
	{
	    if (try_match && keytyped == KEY_OPEN_BACK)
		return TRUE;
	    ++look;
	}

	// 'e' means to check for "else" at start of line and just before the
	// cursor.
	else if (*look == 'e')
	{
	    if (try_match && keytyped == 'e' && curwin->w_cursor.col >= 4)
	    {
		p = ml_get_curline();
		if (skipwhite(p) == p + curwin->w_cursor.col - 4 &&
			STRNCMP(p + curwin->w_cursor.col - 4, "else", 4) == 0)
		    return TRUE;
	    }
	    ++look;
	}

	// ':' only causes an indent if it is at the end of a label or case
	// statement, or when it was before typing the ':' (to fix
	// class::method for C++).
	else if (*look == ':')
	{
	    if (try_match && keytyped == ':')
	    {
		p = ml_get_curline();
		if (cin_iscase(p, FALSE) || cin_isscopedecl(p) || cin_islabel())
		    return TRUE;
		// Need to get the line again after cin_islabel().
		p = ml_get_curline();
		if (curwin->w_cursor.col > 2
			&& p[curwin->w_cursor.col - 1] == ':'
			&& p[curwin->w_cursor.col - 2] == ':')
		{
		    p[curwin->w_cursor.col - 1] = ' ';
		    i = (cin_iscase(p, FALSE) || cin_isscopedecl(p)
							    || cin_islabel());
		    p = ml_get_curline();
		    p[curwin->w_cursor.col - 1] = ':';
		    if (i)
			return TRUE;
		}
	    }
	    ++look;
	}


	// Is it a key in <>, maybe?
	else if (*look == '<')
	{
	    if (try_match)
	    {
		// make up some named keys <o>, <O>, <e>, <0>, <>>, <<>, <*>,
		// <:> and <!> so that people can re-indent on o, O, e, 0, <,
		// >, *, : and ! keys if they really really want to.
		if (vim_strchr((char_u *)"<>!*oOe0:", look[1]) != NULL
						       && keytyped == look[1])
		    return TRUE;

		if (keytyped == get_special_key_code(look + 1))
		    return TRUE;
	    }
	    while (*look && *look != '>')
		look++;
	    while (*look == '>')
		look++;
	}

	// Is it a word: "=word"?
	else if (*look == '=' && look[1] != ',' && look[1] != NUL)
	{
	    ++look;
	    if (*look == '~')
	    {
		icase = TRUE;
		++look;
	    }
	    else
		icase = FALSE;
	    p = vim_strchr(look, ',');
	    if (p == NULL)
		p = look + STRLEN(look);
	    if ((try_match || try_match_word)
		    && curwin->w_cursor.col >= (colnr_T)(p - look))
	    {
		int		match = FALSE;

		if (keytyped == KEY_COMPLETE)
		{
		    char_u	*s;

		    // Just completed a word, check if it starts with "look".
		    // search back for the start of a word.
		    line = ml_get_curline();
		    if (has_mbyte)
		    {
			char_u	*n;

			for (s = line + curwin->w_cursor.col; s > line; s = n)
			{
			    n = mb_prevptr(line, s);
			    if (!vim_iswordp(n))
				break;
			}
		    }
		    else
			for (s = line + curwin->w_cursor.col; s > line; --s)
			    if (!vim_iswordc(s[-1]))
				break;
		    if (s + (p - look) <= line + curwin->w_cursor.col
			    && (icase
				? MB_STRNICMP(s, look, p - look)
				: STRNCMP(s, look, p - look)) == 0)
			match = TRUE;
		}
		else
		    // TODO: multi-byte
		    if (keytyped == (int)p[-1] || (icase && keytyped < 256
			 && TOLOWER_LOC(keytyped) == TOLOWER_LOC((int)p[-1])))
		{
		    line = ml_get_cursor();
		    if ((curwin->w_cursor.col == (colnr_T)(p - look)
				|| !vim_iswordc(line[-(p - look) - 1]))
			    && (icase
				? MB_STRNICMP(line - (p - look), look, p - look)
				: STRNCMP(line - (p - look), look, p - look))
									 == 0)
			match = TRUE;
		}
		if (match && try_match_word && !try_match)
		{
		    // "0=word": Check if there are only blanks before the
		    // word.
		    if (getwhitecols_curline() !=
				     (int)(curwin->w_cursor.col - (p - look)))
			match = FALSE;
		}
		if (match)
		    return TRUE;
	    }
	    look = p;
	}

	// ok, it's a boring generic character.
	else
	{
	    if (try_match && *look == keytyped)
		return TRUE;
	    if (*look != NUL)
		++look;
	}

	// Skip over ", ".
	look = skip_to_option_part(look);
    }
    return FALSE;
}