cin_isinit(void)
{
    char_u	*s;
    static char *skip[] = {"static", "public", "protected", "private"};

    s = cin_skipcomment(ml_get_curline());

    if (cin_starts_with(s, "typedef"))
	s = cin_skipcomment(s + 7);

    for (;;)
    {
	int i, l;

	for (i = 0; i < (int)ARRAY_LENGTH(skip); ++i)
	{
	    l = (int)strlen(skip[i]);
	    if (cin_starts_with(s, skip[i]))
	    {
		s = cin_skipcomment(s + l);
		l = 0;
		break;
	    }
	}
	if (l != 0)
	    break;
    }

    if (cin_starts_with(s, "enum"))
	return TRUE;

    if (cin_ends_in(s, (char_u *)"=", (char_u *)"{"))
	return TRUE;

    return FALSE;
}