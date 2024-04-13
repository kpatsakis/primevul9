do_incsearch_highlighting(
	int		    firstc,
	int		    *search_delim,
	incsearch_state_T   *is_state,
	int		    *skiplen,
	int		    *patlen)
{
    char_u	*cmd;
    cmdmod_T	dummy_cmdmod;
    char_u	*p;
    int		delim_optional = FALSE;
    int		delim;
    char_u	*end;
    char	*dummy;
    exarg_T	ea;
    pos_T	save_cursor;
    int		use_last_pat;
    int		retval = FALSE;
    magic_T     magic = 0;

    *skiplen = 0;
    *patlen = ccline.cmdlen;

    if (!p_is || cmd_silent)
	return FALSE;

    // by default search all lines
    search_first_line = 0;
    search_last_line = MAXLNUM;

    if (firstc == '/' || firstc == '?')
    {
	*search_delim = firstc;
	return TRUE;
    }
    if (firstc != ':')
	return FALSE;

    ++emsg_off;
    CLEAR_FIELD(ea);
    ea.line1 = 1;
    ea.line2 = 1;
    ea.cmd = ccline.cmdbuff;
    ea.addr_type = ADDR_LINES;

    parse_command_modifiers(&ea, &dummy, &dummy_cmdmod, TRUE);

    cmd = skip_range(ea.cmd, TRUE, NULL);
    if (vim_strchr((char_u *)"sgvl", *cmd) == NULL)
	goto theend;

    // Skip over "substitute" to find the pattern separator.
    for (p = cmd; ASCII_ISALPHA(*p); ++p)
	;
    if (*skipwhite(p) == NUL)
	goto theend;

    if (STRNCMP(cmd, "substitute", p - cmd) == 0
	    || STRNCMP(cmd, "smagic", p - cmd) == 0
	    || STRNCMP(cmd, "snomagic", MAX(p - cmd, 3)) == 0
	    || STRNCMP(cmd, "vglobal", p - cmd) == 0)
    {
	if (*cmd == 's' && cmd[1] == 'm')
	    magic_overruled = OPTION_MAGIC_ON;
	else if (*cmd == 's' && cmd[1] == 'n')
	    magic_overruled = OPTION_MAGIC_OFF;
    }
    else if (STRNCMP(cmd, "sort", MAX(p - cmd, 3)) == 0)
    {
	// skip over ! and flags
	if (*p == '!')
	    p = skipwhite(p + 1);
	while (ASCII_ISALPHA(*(p = skipwhite(p))))
	    ++p;
	if (*p == NUL)
	    goto theend;
    }
    else if (STRNCMP(cmd, "vimgrep", MAX(p - cmd, 3)) == 0
	|| STRNCMP(cmd, "vimgrepadd", MAX(p - cmd, 8)) == 0
	|| STRNCMP(cmd, "lvimgrep", MAX(p - cmd, 2)) == 0
	|| STRNCMP(cmd, "lvimgrepadd", MAX(p - cmd, 9)) == 0
	|| STRNCMP(cmd, "global", p - cmd) == 0)
    {
	// skip over "!"
	if (*p == '!')
	{
	    p++;
	    if (*skipwhite(p) == NUL)
		goto theend;
	}
	if (*cmd != 'g')
	    delim_optional = TRUE;
    }
    else
	goto theend;

    p = skipwhite(p);
    delim = (delim_optional && vim_isIDc(*p)) ? ' ' : *p++;
    *search_delim = delim;
    end = skip_regexp_ex(p, delim, magic_isset(), NULL, NULL, &magic);

    use_last_pat = end == p && *end == delim;

    if (end == p && !use_last_pat)
	goto theend;

    // Don't do 'hlsearch' highlighting if the pattern matches everything.
    if (!use_last_pat)
    {
	char c = *end;
	int  empty;

	*end = NUL;
	empty = empty_pattern_magic(p, STRLEN(p), magic);
	*end = c;
	if (empty)
	    goto theend;
    }

    // found a non-empty pattern or //
    *skiplen = (int)(p - ccline.cmdbuff);
    *patlen = (int)(end - p);

    // parse the address range
    save_cursor = curwin->w_cursor;
    curwin->w_cursor = is_state->search_start;
    parse_cmd_address(&ea, &dummy, TRUE);
    if (ea.addr_count > 0)
    {
	// Allow for reverse match.
	if (ea.line2 < ea.line1)
	{
	    search_first_line = ea.line2;
	    search_last_line = ea.line1;
	}
	else
	{
	    search_first_line = ea.line1;
	    search_last_line = ea.line2;
	}
    }
    else if (cmd[0] == 's' && cmd[1] != 'o')
    {
	// :s defaults to the current line
	search_first_line = curwin->w_cursor.lnum;
	search_last_line = curwin->w_cursor.lnum;
    }

    curwin->w_cursor = save_cursor;
    retval = TRUE;
theend:
    --emsg_off;
    return retval;
}