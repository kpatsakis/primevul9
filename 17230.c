transstr(char_u *s)
{
    char_u	*res;
    char_u	*p;
    int		l, len, c;
    char_u	hexbuf[11];

    if (has_mbyte)
    {
	// Compute the length of the result, taking account of unprintable
	// multi-byte characters.
	len = 0;
	p = s;
	while (*p != NUL)
	{
	    if ((l = (*mb_ptr2len)(p)) > 1)
	    {
		c = (*mb_ptr2char)(p);
		p += l;
		if (vim_isprintc(c))
		    len += l;
		else
		{
		    transchar_hex(hexbuf, c);
		    len += (int)STRLEN(hexbuf);
		}
	    }
	    else
	    {
		l = byte2cells(*p++);
		if (l > 0)
		    len += l;
		else
		    len += 4;	// illegal byte sequence
	    }
	}
	res = alloc(len + 1);
    }
    else
	res = alloc(vim_strsize(s) + 1);
    if (res != NULL)
    {
	*res = NUL;
	p = s;
	while (*p != NUL)
	{
	    if (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
	    {
		c = (*mb_ptr2char)(p);
		if (vim_isprintc(c))
		    STRNCAT(res, p, l);	// append printable multi-byte char
		else
		    transchar_hex(res + STRLEN(res), c);
		p += l;
	    }
	    else
		STRCAT(res, transchar_byte(*p++));
	}
    }
    return res;
}