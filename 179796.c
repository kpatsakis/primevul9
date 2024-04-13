cin_iscase(
    char_u *s,
    int strict) // Allow relaxed check of case statement for JS
{
    s = cin_skipcomment(s);
    if (cin_starts_with(s, "case"))
    {
	for (s += 4; *s; ++s)
	{
	    s = cin_skipcomment(s);
	    if (*s == NUL)
		break;
	    if (*s == ':')
	    {
		if (s[1] == ':')	// skip over "::" for C++
		    ++s;
		else
		    return TRUE;
	    }
	    if (*s == '\'' && s[1] && s[2] == '\'')
		s += 2;			// skip over ':'
	    else if (*s == '/' && (s[1] == '*' || s[1] == '/'))
		return FALSE;		// stop at comment
	    else if (*s == '"')
	    {
		// JS etc.
		if (strict)
		    return FALSE;		// stop at string
		else
		    return TRUE;
	    }
	}
	return FALSE;
    }

    if (cin_isdefault(s))
	return TRUE;
    return FALSE;
}