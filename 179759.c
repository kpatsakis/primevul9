cin_first_id_amount(void)
{
    char_u	*line, *p, *s;
    int		len;
    pos_T	fp;
    colnr_T	col;

    line = ml_get_curline();
    p = skipwhite(line);
    len = (int)(skiptowhite(p) - p);
    if (len == 6 && STRNCMP(p, "static", 6) == 0)
    {
	p = skipwhite(p + 6);
	len = (int)(skiptowhite(p) - p);
    }
    if (len == 6 && STRNCMP(p, "struct", 6) == 0)
	p = skipwhite(p + 6);
    else if (len == 4 && STRNCMP(p, "enum", 4) == 0)
	p = skipwhite(p + 4);
    else if ((len == 8 && STRNCMP(p, "unsigned", 8) == 0)
	    || (len == 6 && STRNCMP(p, "signed", 6) == 0))
    {
	s = skipwhite(p + len);
	if ((STRNCMP(s, "int", 3) == 0 && VIM_ISWHITE(s[3]))
		|| (STRNCMP(s, "long", 4) == 0 && VIM_ISWHITE(s[4]))
		|| (STRNCMP(s, "short", 5) == 0 && VIM_ISWHITE(s[5]))
		|| (STRNCMP(s, "char", 4) == 0 && VIM_ISWHITE(s[4])))
	    p = s;
    }
    for (len = 0; vim_isIDc(p[len]); ++len)
	;
    if (len == 0 || !VIM_ISWHITE(p[len]) || cin_nocode(p))
	return 0;

    p = skipwhite(p + len);
    fp.lnum = curwin->w_cursor.lnum;
    fp.col = (colnr_T)(p - line);
    getvcol(curwin, &fp, &col, NULL, NULL);
    return (int)col;
}