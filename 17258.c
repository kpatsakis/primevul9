skipwhite_and_nl(char_u *q)
{
    char_u	*p = q;

    while (VIM_ISWHITE(*p) || *p == NL)
	++p;
    return p;
}