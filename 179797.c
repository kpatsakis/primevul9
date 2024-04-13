cin_ispreproc(char_u *s)
{
    if (*skipwhite(s) == '#')
	return TRUE;
    return FALSE;
}