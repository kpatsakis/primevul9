has_special_wildchar(char_u *p)
{
    for ( ; *p; MB_PTR_ADV(p))
    {
	// Disallow line break characters.
	if (*p == '\r' || *p == '\n')
	    break;
	// Allow for escaping.
	if (*p == '\\' && p[1] != NUL && p[1] != '\r' && p[1] != '\n')
	    ++p;
	else if (vim_strchr((char_u *)SPECIAL_WILDCHAR, *p) != NULL)
	{
	    // A { must be followed by a matching }.
	    if (*p == '{' && vim_strchr(p, '}') == NULL)
		continue;
	    // A quote and backtick must be followed by another one.
	    if ((*p == '`' || *p == '\'') && vim_strchr(p, *p) == NULL)
		continue;
	    return TRUE;
	}
    }
    return FALSE;
}