getdigits(char_u **pp)
{
    char_u	*p;
    long	retval;

    p = *pp;
    retval = atol((char *)p);
    if (*p == '-')		// skip negative sign
	++p;
    p = skipdigits(p);		// skip to next non-digit
    *pp = p;
    return retval;
}