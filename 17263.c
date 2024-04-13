skipwhite(char_u *q)
{
    char_u	*p = q;

    while (VIM_ISWHITE(*p))
	++p;
    return p;
}