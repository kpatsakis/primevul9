cin_isfuncdecl(
    char_u	**sp,
    linenr_T	first_lnum,
    linenr_T	min_lnum)
{
    char_u	*s;
    linenr_T	lnum = first_lnum;
    linenr_T	save_lnum = curwin->w_cursor.lnum;
    int		retval = FALSE;
    pos_T	*trypos;
    int		just_started = TRUE;

    if (sp == NULL)
	s = ml_get(lnum);
    else
	s = *sp;

    curwin->w_cursor.lnum = lnum;
    if (find_last_paren(s, '(', ')')
	&& (trypos = find_match_paren(curbuf->b_ind_maxparen)) != NULL)
    {
	lnum = trypos->lnum;
	if (lnum < min_lnum)
	{
	    curwin->w_cursor.lnum = save_lnum;
	    return FALSE;
	}

	s = ml_get(lnum);
    }
    curwin->w_cursor.lnum = save_lnum;

    // Ignore line starting with #.
    if (cin_ispreproc(s))
	return FALSE;

    while (*s && *s != '(' && *s != ';' && *s != '\'' && *s != '"')
    {
	if (cin_iscomment(s))	// ignore comments
	    s = cin_skipcomment(s);
	else if (*s == ':')
	{
	    if (*(s + 1) == ':')
		s += 2;
	    else
		// To avoid a mistake in the following situation:
		// A::A(int a, int b)
		//     : a(0)  // <--not a function decl
		//     , b(0)
		// {...
		return FALSE;
	}
	else
	    ++s;
    }
    if (*s != '(')
	return FALSE;		// ';', ' or "  before any () or no '('

    while (*s && *s != ';' && *s != '\'' && *s != '"')
    {
	if (*s == ')' && cin_nocode(s + 1))
	{
	    // ')' at the end: may have found a match
	    // Check for the previous line not to end in a backslash:
	    //       #if defined(x) && {backslash}
	    //		 defined(y)
	    lnum = first_lnum - 1;
	    s = ml_get(lnum);
	    if (*s == NUL || s[STRLEN(s) - 1] != '\\')
		retval = TRUE;
	    goto done;
	}
	if ((*s == ',' && cin_nocode(s + 1)) || s[1] == NUL || cin_nocode(s))
	{
	    int comma = (*s == ',');

	    // ',' at the end: continue looking in the next line.
	    // At the end: check for ',' in the next line, for this style:
	    // func(arg1
	    //       , arg2)
	    for (;;)
	    {
		if (lnum >= curbuf->b_ml.ml_line_count)
		    break;
		s = ml_get(++lnum);
		if (!cin_ispreproc(s))
		    break;
	    }
	    if (lnum >= curbuf->b_ml.ml_line_count)
		break;
	    // Require a comma at end of the line or a comma or ')' at the
	    // start of next line.
	    s = skipwhite(s);
	    if (!just_started && (!comma && *s != ',' && *s != ')'))
		break;
	    just_started = FALSE;
	}
	else if (cin_iscomment(s))	// ignore comments
	    s = cin_skipcomment(s);
	else
	{
	    ++s;
	    just_started = FALSE;
	}
    }

done:
    if (lnum != first_lnum && sp != NULL)
	*sp = ml_get(first_lnum);

    return retval;
}