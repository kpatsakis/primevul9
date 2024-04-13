skiptowhite_esc(char_u *p)
{
    while (*p != ' ' && *p != '\t' && *p != NUL)
    {
	if ((*p == '\\' || *p == Ctrl_V) && *(p + 1) != NUL)
	    ++p;
	++p;
    }
    return p;
}