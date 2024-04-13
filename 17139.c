getdigits_quoted(char_u **pp)
{
    char_u	*p = *pp;
    long	retval = 0;

    if (*p == '-')
	++p;
    while (VIM_ISDIGIT(*p))
    {
	if (retval >= LONG_MAX / 10 - 10)
	    retval = LONG_MAX;
	else
	    retval = retval * 10 - '0' + *p;
	++p;
	if (in_vim9script() && *p == '\'' && VIM_ISDIGIT(p[1]))
	    ++p;
    }
    if (**pp == '-')
    {
	if (retval == LONG_MAX)
	    retval = LONG_MIN;
	else
	    retval = -retval;
    }
    *pp = p;
    return retval;
}