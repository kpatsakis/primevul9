skiptohex(char_u *q)
{
    char_u	*p = q;

    while (*p != NUL && !vim_isxdigit(*p))	// skip to next digit
	++p;
    return p;
}