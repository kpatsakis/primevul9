vim_strnsize(char_u *s, int len)
{
    int		size = 0;

    while (*s != NUL && --len >= 0)
	if (has_mbyte)
	{
	    int	    l = (*mb_ptr2len)(s);

	    size += ptr2cells(s);
	    s += l;
	    len -= l - 1;
	}
	else
	    size += byte2cells(*s++);

    return size;
}