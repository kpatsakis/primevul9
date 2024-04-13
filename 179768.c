cin_islabel_skip(char_u **s)
{
    if (!vim_isIDc(**s))	    // need at least one ID character
	return FALSE;

    while (vim_isIDc(**s))
	(*s)++;

    *s = cin_skipcomment(*s);

    // "::" is not a label, it's C++
    return (**s == ':' && *++*s != ':');
}