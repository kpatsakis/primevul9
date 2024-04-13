cin_is_cpp_extern_c(char_u *s)
{
    char_u	*p;
    int		has_string_literal = FALSE;

    s = cin_skipcomment(s);
    if (STRNCMP(s, "extern", 6) == 0 && (s[6] == NUL || !vim_iswordc(s[6])))
    {
	p = cin_skipcomment(skipwhite(s + 6));
	while (*p != NUL)
	{
	    if (VIM_ISWHITE(*p))
	    {
		p = cin_skipcomment(skipwhite(p));
	    }
	    else if (*p == '{')
	    {
		break;
	    }
	    else if (p[0] == '"' && p[1] == 'C' && p[2] == '"')
	    {
		if (has_string_literal)
		    return FALSE;
		has_string_literal = TRUE;
		p += 3;
	    }
	    else if (p[0] == '"' && p[1] == 'C' && p[2] == '+' && p[3] == '+'
		    && p[4] == '"')
	    {
		if (has_string_literal)
		    return FALSE;
		has_string_literal = TRUE;
		p += 5;
	    }
	    else
	    {
		return FALSE;
	    }
	}
	return has_string_literal ? TRUE : FALSE;
    }
    return FALSE;
}