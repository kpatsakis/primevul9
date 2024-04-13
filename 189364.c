check_opt_wim(void)
{
    char_u	new_wim_flags[4];
    char_u	*p;
    int		i;
    int		idx = 0;

    for (i = 0; i < 4; ++i)
	new_wim_flags[i] = 0;

    for (p = p_wim; *p; ++p)
    {
	for (i = 0; ASCII_ISALPHA(p[i]); ++i)
	    ;
	if (p[i] != NUL && p[i] != ',' && p[i] != ':')
	    return FAIL;
	if (i == 7 && STRNCMP(p, "longest", 7) == 0)
	    new_wim_flags[idx] |= WIM_LONGEST;
	else if (i == 4 && STRNCMP(p, "full", 4) == 0)
	    new_wim_flags[idx] |= WIM_FULL;
	else if (i == 4 && STRNCMP(p, "list", 4) == 0)
	    new_wim_flags[idx] |= WIM_LIST;
	else
	    return FAIL;
	p += i;
	if (*p == NUL)
	    break;
	if (*p == ',')
	{
	    if (idx == 3)
		return FAIL;
	    ++idx;
	}
    }

    /* fill remaining entries with last flag */
    while (idx < 3)
    {
	new_wim_flags[idx + 1] = new_wim_flags[idx];
	++idx;
    }

    /* only when there are no errors, wim_flags[] is changed */
    for (i = 0; i < 4; ++i)
	wim_flags[i] = new_wim_flags[i];
    return OK;
}