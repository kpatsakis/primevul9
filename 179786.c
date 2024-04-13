cin_skip2pos(pos_T *trypos)
{
    char_u	*line;
    char_u	*p;
    char_u	*new_p;

    p = line = ml_get(trypos->lnum);
    while (*p && (colnr_T)(p - line) < trypos->col)
    {
	if (cin_iscomment(p))
	    p = cin_skipcomment(p);
	else
	{
	    new_p = skip_string(p);
	    if (new_p == p)
		++p;
	    else
		p = new_p;
	}
    }
    return (int)(p - line);
}