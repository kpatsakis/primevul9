backslash_halve(char_u *p)
{
    for ( ; *p; ++p)
	if (rem_backslash(p))
	    STRMOVE(p, p + 1);
}