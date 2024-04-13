cin_ispreproc_cont(char_u **pp, linenr_T *lnump, int *amount)
{
    char_u	*line = *pp;
    linenr_T	lnum = *lnump;
    int		retval = FALSE;
    int		candidate_amount = *amount;

    if (*line != NUL && line[STRLEN(line) - 1] == '\\')
	candidate_amount = get_indent_lnum(lnum);

    for (;;)
    {
	if (cin_ispreproc(line))
	{
	    retval = TRUE;
	    *lnump = lnum;
	    break;
	}
	if (lnum == 1)
	    break;
	line = ml_get(--lnum);
	if (*line == NUL || line[STRLEN(line) - 1] != '\\')
	    break;
    }

    if (lnum != *lnump)
	*pp = ml_get(*lnump);
    if (retval)
	*amount = candidate_amount;
    return retval;
}