fullpathcmp(
    char_u *s1,
    char_u *s2,
    int	    checkname,		// when both don't exist, check file names
    int	    expandenv)
{
#ifdef UNIX
    char_u	    exp1[MAXPATHL];
    char_u	    full1[MAXPATHL];
    char_u	    full2[MAXPATHL];
    stat_T	    st1, st2;
    int		    r1, r2;

    if (expandenv)
	expand_env(s1, exp1, MAXPATHL);
    else
	vim_strncpy(exp1, s1, MAXPATHL - 1);
    r1 = mch_stat((char *)exp1, &st1);
    r2 = mch_stat((char *)s2, &st2);
    if (r1 != 0 && r2 != 0)
    {
	// if mch_stat() doesn't work, may compare the names
	if (checkname)
	{
	    if (fnamecmp(exp1, s2) == 0)
		return FPC_SAMEX;
	    r1 = vim_FullName(exp1, full1, MAXPATHL, FALSE);
	    r2 = vim_FullName(s2, full2, MAXPATHL, FALSE);
	    if (r1 == OK && r2 == OK && fnamecmp(full1, full2) == 0)
		return FPC_SAMEX;
	}
	return FPC_NOTX;
    }
    if (r1 != 0 || r2 != 0)
	return FPC_DIFFX;
    if (st1.st_dev == st2.st_dev && st1.st_ino == st2.st_ino)
	return FPC_SAME;
    return FPC_DIFF;
#else
    char_u  *exp1;		// expanded s1
    char_u  *full1;		// full path of s1
    char_u  *full2;		// full path of s2
    int	    retval = FPC_DIFF;
    int	    r1, r2;

    // allocate one buffer to store three paths (alloc()/free() is slow!)
    if ((exp1 = alloc(MAXPATHL * 3)) != NULL)
    {
	full1 = exp1 + MAXPATHL;
	full2 = full1 + MAXPATHL;

	if (expandenv)
	    expand_env(s1, exp1, MAXPATHL);
	else
	    vim_strncpy(exp1, s1, MAXPATHL - 1);
	r1 = vim_FullName(exp1, full1, MAXPATHL, FALSE);
	r2 = vim_FullName(s2, full2, MAXPATHL, FALSE);

	// If vim_FullName() fails, the file probably doesn't exist.
	if (r1 != OK && r2 != OK)
	{
	    if (checkname && fnamecmp(exp1, s2) == 0)
		retval = FPC_SAMEX;
	    else
		retval = FPC_NOTX;
	}
	else if (r1 != OK || r2 != OK)
	    retval = FPC_DIFFX;
	else if (fnamecmp(full1, full2))
	    retval = FPC_DIFF;
	else
	    retval = FPC_SAME;
	vim_free(exp1);
    }
    return retval;
#endif
}