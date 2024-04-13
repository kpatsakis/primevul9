find_line_comment(void) // XXX
{
    static pos_T pos;
    char_u	 *line;
    char_u	 *p;

    pos = curwin->w_cursor;
    while (--pos.lnum > 0)
    {
	line = ml_get(pos.lnum);
	p = skipwhite(line);
	if (cin_islinecomment(p))
	{
	    pos.col = (int)(p - line);
	    return &pos;
	}
	if (*p != NUL)
	    break;
    }
    return NULL;
}