skipbin(char_u *q)
{
    char_u	*p = q;

    while (vim_isbdigit(*p))	// skip to next non-digit
	++p;
    return p;
}