cin_skipcomment(char_u *s)
{
    while (*s)
    {
	char_u *prev_s = s;

	s = skipwhite(s);

	// Perl/shell # comment comment continues until eol.  Require a space
	// before # to avoid recognizing $#array.
	if (curbuf->b_ind_hash_comment != 0 && s != prev_s && *s == '#')
	{
	    s += STRLEN(s);
	    break;
	}
	if (*s != '/')
	    break;
	++s;
	if (*s == '/')		// slash-slash comment continues till eol
	{
	    s += STRLEN(s);
	    break;
	}
	if (*s != '*')
	    break;
	for (++s; *s; ++s)	// skip slash-star comment
	    if (s[0] == '*' && s[1] == '/')
	    {
		s += 2;
		break;
	    }
    }
    return s;
}