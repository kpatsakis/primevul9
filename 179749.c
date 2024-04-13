cin_is_cpp_namespace(char_u *s)
{
    char_u	*p;
    int		has_name = FALSE;
    int		has_name_start = FALSE;

    s = cin_skipcomment(s);

    if (STRNCMP(s, "inline", 6) == 0 && (s[6] == NUL || !vim_iswordc(s[6])))
	s = cin_skipcomment(skipwhite(s + 6));

    if (STRNCMP(s, "namespace", 9) == 0 && (s[9] == NUL || !vim_iswordc(s[9])))
    {
	p = cin_skipcomment(skipwhite(s + 9));
	while (*p != NUL)
	{
	    if (VIM_ISWHITE(*p))
	    {
		has_name = TRUE; // found end of a name
		p = cin_skipcomment(skipwhite(p));
	    }
	    else if (*p == '{')
	    {
		break;
	    }
	    else if (vim_iswordc(*p))
	    {
		has_name_start = TRUE;
		if (has_name)
		    return FALSE; // word character after skipping past name
		++p;
	    }
	    else if (p[0] == ':' && p[1] == ':' && vim_iswordc(p[2]))
	    {
		if (!has_name_start || has_name)
		    return FALSE;
		// C++ 17 nested namespace
		p += 3;
	    }
	    else
	    {
		return FALSE;
	    }
	}
	return TRUE;
    }
    return FALSE;
}