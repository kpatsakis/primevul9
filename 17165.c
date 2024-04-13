skiptobin(char_u *q)
{
    char_u	*p = q;

    while (*p != NUL && !vim_isbdigit(*p))	// skip to next digit
	++p;
    return p;
}