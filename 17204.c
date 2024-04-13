skipdigits(char_u *q)
{
    char_u	*p = q;

    while (VIM_ISDIGIT(*p))	// skip to next non-digit
	++p;
    return p;
}