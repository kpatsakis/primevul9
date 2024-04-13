vim_fexists(char_u *fname)
{
    stat_T st;

    if (mch_stat((char *)fname, &st))
	return FALSE;
    return TRUE;
}