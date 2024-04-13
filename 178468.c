skip_anyof(char_u *p)
{
    int		l;

    if (*p == '^')	// Complement of range.
	++p;
    if (*p == ']' || *p == '-')
	++p;
    while (*p != NUL && *p != ']')
    {
	if (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
	    p += l;
	else
	    if (*p == '-')
	    {
		++p;
		if (*p != ']' && *p != NUL)
		    MB_PTR_ADV(p);
	    }
	else if (*p == '\\'
		&& !reg_cpo_bsl
		&& (vim_strchr(REGEXP_INRANGE, p[1]) != NULL
		    || (!reg_cpo_lit && vim_strchr(REGEXP_ABBR, p[1]) != NULL)))
	    p += 2;
	else if (*p == '[')
	{
	    if (get_char_class(&p) == CLASS_NONE
		    && get_equi_class(&p) == 0
		    && get_coll_element(&p) == 0
		    && *p != NUL)
		++p; // it is not a class name and not NUL
	}
	else
	    ++p;
    }

    return p;
}