getfpermst(stat_T *st, char_u *perm)
{
    char_u	    flags[] = "rwx";
    int		    i;

    for (i = 0; i < 9; i++)
    {
	if (st->st_mode & (1 << (8 - i)))
	    perm[i] = flags[i % 3];
	else
	    perm[i] = '-';
    }
    return perm;
}