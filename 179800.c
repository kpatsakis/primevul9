cin_ends_in(char_u *s, char_u *find, char_u *ignore)
{
    char_u	*p = s;
    char_u	*r;
    int		len = (int)STRLEN(find);

    while (*p != NUL)
    {
	p = cin_skipcomment(p);
	if (STRNCMP(p, find, len) == 0)
	{
	    r = skipwhite(p + len);
	    if (ignore != NULL && STRNCMP(r, ignore, STRLEN(ignore)) == 0)
		r = skipwhite(r + STRLEN(ignore));
	    if (cin_nocode(r))
		return TRUE;
	}
	if (*p != NUL)
	    ++p;
    }
    return FALSE;
}