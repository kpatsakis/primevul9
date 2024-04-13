in_html_tag(
    int		end_tag)
{
    char_u	*line = ml_get_curline();
    char_u	*p;
    int		c;
    int		lc = NUL;
    pos_T	pos;

    if (enc_dbcs)
    {
	char_u	*lp = NULL;

	// We search forward until the cursor, because searching backwards is
	// very slow for DBCS encodings.
	for (p = line; p < line + curwin->w_cursor.col; MB_PTR_ADV(p))
	    if (*p == '>' || *p == '<')
	    {
		lc = *p;
		lp = p;
	    }
	if (*p != '<')	    // check for '<' under cursor
	{
	    if (lc != '<')
		return FALSE;
	    p = lp;
	}
    }
    else
    {
	for (p = line + curwin->w_cursor.col; p > line; )
	{
	    if (*p == '<')	// find '<' under/before cursor
		break;
	    MB_PTR_BACK(line, p);
	    if (*p == '>')	// find '>' before cursor
		break;
	}
	if (*p != '<')
	    return FALSE;
    }

    pos.lnum = curwin->w_cursor.lnum;
    pos.col = (colnr_T)(p - line);

    MB_PTR_ADV(p);
    if (end_tag)
	// check that there is a '/' after the '<'
	return *p == '/';

    // check that there is no '/' after the '<'
    if (*p == '/')
	return FALSE;

    // check that the matching '>' is not preceded by '/'
    for (;;)
    {
	if (inc(&pos) < 0)
	    return FALSE;
	c = *ml_get_pos(&pos);
	if (c == '>')
	    break;
	lc = c;
    }
    return lc != '/';
}