same_leader(
    linenr_T lnum,
    int	    leader1_len,
    char_u  *leader1_flags,
    int	    leader2_len,
    char_u  *leader2_flags)
{
    int	    idx1 = 0, idx2 = 0;
    char_u  *p;
    char_u  *line1;
    char_u  *line2;

    if (leader1_len == 0)
	return (leader2_len == 0);

    // If first leader has 'f' flag, the lines can be joined only if the
    // second line does not have a leader.
    // If first leader has 'e' flag, the lines can never be joined.
    // If first leader has 's' flag, the lines can only be joined if there is
    // some text after it and the second line has the 'm' flag.
    if (leader1_flags != NULL)
    {
	for (p = leader1_flags; *p && *p != ':'; ++p)
	{
	    if (*p == COM_FIRST)
		return (leader2_len == 0);
	    if (*p == COM_END)
		return FALSE;
	    if (*p == COM_START)
	    {
		if (*(ml_get(lnum) + leader1_len) == NUL)
		    return FALSE;
		if (leader2_flags == NULL || leader2_len == 0)
		    return FALSE;
		for (p = leader2_flags; *p && *p != ':'; ++p)
		    if (*p == COM_MIDDLE)
			return TRUE;
		return FALSE;
	    }
	}
    }

    // Get current line and next line, compare the leaders.
    // The first line has to be saved, only one line can be locked at a time.
    line1 = vim_strsave(ml_get(lnum));
    if (line1 != NULL)
    {
	for (idx1 = 0; VIM_ISWHITE(line1[idx1]); ++idx1)
	    ;
	line2 = ml_get(lnum + 1);
	for (idx2 = 0; idx2 < leader2_len; ++idx2)
	{
	    if (!VIM_ISWHITE(line2[idx2]))
	    {
		if (line1[idx1++] != line2[idx2])
		    break;
	    }
	    else
		while (VIM_ISWHITE(line1[idx1]))
		    ++idx1;
	}
	vim_free(line1);
    }
    return (idx2 == leader2_len && idx1 == leader1_len);
}