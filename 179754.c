cin_has_js_key(char_u *text)
{
    char_u *s = skipwhite(text);
    int	    quote = -1;

    if (*s == '\'' || *s == '"')
    {
	// can be 'key': or "key":
	quote = *s;
	++s;
    }
    if (!vim_isIDc(*s))	    // need at least one ID character
	return FALSE;

    while (vim_isIDc(*s))
	++s;
    if (*s == quote)
	++s;

    s = cin_skipcomment(s);

    // "::" is not a label, it's C++
    return (*s == ':' && s[1] != ':');
}