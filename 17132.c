dir_of_file_exists(char_u *fname)
{
    char_u	*p;
    int		c;
    int		retval;

    p = gettail_sep(fname);
    if (p == fname)
	return TRUE;
    c = *p;
    *p = NUL;
    retval = mch_isdir(fname);
    *p = c;
    return retval;
}