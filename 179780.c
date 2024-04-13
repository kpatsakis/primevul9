cin_get_equal_amount(linenr_T lnum)
{
    char_u	*line;
    char_u	*s;
    colnr_T	col;
    pos_T	fp;

    if (lnum > 1)
    {
	line = ml_get(lnum - 1);
	if (*line != NUL && line[STRLEN(line) - 1] == '\\')
	    return -1;
    }

    line = s = ml_get(lnum);
    while (*s != NUL && vim_strchr((char_u *)"=;{}\"'", *s) == NULL)
    {
	if (cin_iscomment(s))	// ignore comments
	    s = cin_skipcomment(s);
	else
	    ++s;
    }
    if (*s != '=')
	return 0;

    s = skipwhite(s + 1);
    if (cin_nocode(s))
	return 0;

    if (*s == '"')	// nice alignment for continued strings
	++s;

    fp.lnum = lnum;
    fp.col = (colnr_T)(s - line);
    getvcol(curwin, &fp, &col, NULL, NULL);
    return (int)col;
}