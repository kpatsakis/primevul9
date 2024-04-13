skiphex(char_u *q)
{
    char_u	*p = q;

    while (vim_isxdigit(*p))	// skip to next non-digit
	++p;
    return p;
}