vim_fnamecmp(char_u *x, char_u *y)
{
#ifdef BACKSLASH_IN_FILENAME
    return vim_fnamencmp(x, y, MAXPATHL);
#else
    if (p_fic)
	return MB_STRICMP(x, y);
    return STRCMP(x, y);
#endif
}