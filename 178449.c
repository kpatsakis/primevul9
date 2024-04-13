get_coll_element(char_u **pp)
{
    int		c;
    int		l = 1;
    char_u	*p = *pp;

    if (p[0] != NUL && p[1] == '.' && p[2] != NUL)
    {
	if (has_mbyte)
	    l = (*mb_ptr2len)(p + 2);
	if (p[l + 2] == '.' && p[l + 3] == ']')
	{
	    if (has_mbyte)
		c = mb_ptr2char(p + 2);
	    else
		c = p[2];
	    *pp += l + 4;
	    return c;
	}
    }
    return 0;
}