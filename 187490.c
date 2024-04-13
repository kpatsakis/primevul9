fmt_check_par(
    linenr_T	lnum,
    int		*leader_len,
    char_u	**leader_flags,
    int		do_comments)
{
    char_u	*flags = NULL;	    // init for GCC
    char_u	*ptr;

    ptr = ml_get(lnum);
    if (do_comments)
	*leader_len = get_leader_len(ptr, leader_flags, FALSE, TRUE);
    else
	*leader_len = 0;

    if (*leader_len > 0)
    {
	// Search for 'e' flag in comment leader flags.
	flags = *leader_flags;
	while (*flags && *flags != ':' && *flags != COM_END)
	    ++flags;
    }

    return (*skipwhite(ptr + *leader_len) == NUL
	    || (*leader_len > 0 && *flags == COM_END)
	    || startPS(lnum, NUL, FALSE));
}