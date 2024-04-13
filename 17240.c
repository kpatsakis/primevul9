vim_fnamencmp(char_u *x, char_u *y, size_t len)
{
#ifdef BACKSLASH_IN_FILENAME
    char_u	*px = x;
    char_u	*py = y;
    int		cx = NUL;
    int		cy = NUL;

    while (len > 0)
    {
	cx = PTR2CHAR(px);
	cy = PTR2CHAR(py);
	if (cx == NUL || cy == NUL
	    || ((p_fic ? MB_TOLOWER(cx) != MB_TOLOWER(cy) : cx != cy)
		&& !(cx == '/' && cy == '\\')
		&& !(cx == '\\' && cy == '/')))
	    break;
	len -= mb_ptr2len(px);
	px += mb_ptr2len(px);
	py += mb_ptr2len(py);
    }
    if (len == 0)
	return 0;
    return (cx - cy);
#else
    if (p_fic)
	return MB_STRNICMP(x, y, len);
    return STRNCMP(x, y, len);
#endif
}