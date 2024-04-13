skiptowhite(char_u *p)
{
    while (*p != ' ' && *p != '\t' && *p != NUL)
	++p;
    return p;
}