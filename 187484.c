paragraph_start(linenr_T lnum)
{
    char_u	*p;
    int		leader_len = 0;		// leader len of current line
    char_u	*leader_flags = NULL;	// flags for leader of current line
    int		next_leader_len;	// leader len of next line
    char_u	*next_leader_flags;	// flags for leader of next line
    int		do_comments;		// format comments

    if (lnum <= 1)
	return TRUE;		// start of the file

    p = ml_get(lnum - 1);
    if (*p == NUL)
	return TRUE;		// after empty line

    do_comments = has_format_option(FO_Q_COMS);
    if (fmt_check_par(lnum - 1, &leader_len, &leader_flags, do_comments))
	return TRUE;		// after non-paragraph line

    if (fmt_check_par(lnum, &next_leader_len, &next_leader_flags, do_comments))
	return TRUE;		// "lnum" is not a paragraph line

    if (has_format_option(FO_WHITE_PAR) && !ends_in_white(lnum - 1))
	return TRUE;		// missing trailing space in previous line.

    if (has_format_option(FO_Q_NUMBER) && (get_number_indent(lnum) > 0))
	return TRUE;		// numbered item starts in "lnum".

    if (!same_leader(lnum - 1, leader_len, leader_flags,
					  next_leader_len, next_leader_flags))
	return TRUE;		// change of comment leader.

    return FALSE;
}