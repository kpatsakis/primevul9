cin_isterminated(
    char_u	*s,
    int		incl_open,	// include '{' at the end as terminator
    int		incl_comma)	// recognize a trailing comma
{
    char_u	found_start = 0;
    unsigned	n_open = 0;
    int		is_else = FALSE;

    s = cin_skipcomment(s);

    if (*s == '{' || (*s == '}' && !cin_iselse(s)))
	found_start = *s;

    if (!found_start)
	is_else = cin_iselse(s);

    while (*s)
    {
	// skip over comments, "" strings and 'c'haracters
	s = skip_string(cin_skipcomment(s));
	if (*s == '}' && n_open > 0)
	    --n_open;
	if ((!is_else || n_open == 0)
		&& (*s == ';' || *s == '}' || (incl_comma && *s == ','))
		&& cin_nocode(s + 1))
	    return *s;
	else if (*s == '{')
	{
	    if (incl_open && cin_nocode(s + 1))
		return *s;
	    else
		++n_open;
	}

	if (*s)
	    s++;
    }
    return found_start;
}