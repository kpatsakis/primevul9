cmdline_paste(
    int regname,
    int literally,	// Insert text literally instead of "as typed"
    int remcr)		// remove trailing CR
{
    long		i;
    char_u		*arg;
    char_u		*p;
    int			allocated;

    // check for valid regname; also accept special characters for CTRL-R in
    // the command line
    if (regname != Ctrl_F && regname != Ctrl_P && regname != Ctrl_W
	    && regname != Ctrl_A && regname != Ctrl_L
	    && !valid_yank_reg(regname, FALSE))
	return FAIL;

    // A register containing CTRL-R can cause an endless loop.  Allow using
    // CTRL-C to break the loop.
    line_breakcheck();
    if (got_int)
	return FAIL;

#ifdef FEAT_CLIPBOARD
    regname = may_get_selection(regname);
#endif

    // Need to  set "textwinlock" to avoid nasty things like going to another
    // buffer when evaluating an expression.
    ++textwinlock;
    i = get_spec_reg(regname, &arg, &allocated, TRUE);
    --textwinlock;

    if (i)
    {
	// Got the value of a special register in "arg".
	if (arg == NULL)
	    return FAIL;

	// When 'incsearch' is set and CTRL-R CTRL-W used: skip the duplicate
	// part of the word.
	p = arg;
	if (p_is && regname == Ctrl_W)
	{
	    char_u  *w;
	    int	    len;

	    // Locate start of last word in the cmd buffer.
	    for (w = ccline.cmdbuff + ccline.cmdpos; w > ccline.cmdbuff; )
	    {
		if (has_mbyte)
		{
		    len = (*mb_head_off)(ccline.cmdbuff, w - 1) + 1;
		    if (!vim_iswordc(mb_ptr2char(w - len)))
			break;
		    w -= len;
		}
		else
		{
		    if (!vim_iswordc(w[-1]))
			break;
		    --w;
		}
	    }
	    len = (int)((ccline.cmdbuff + ccline.cmdpos) - w);
	    if (p_ic ? STRNICMP(w, arg, len) == 0 : STRNCMP(w, arg, len) == 0)
		p += len;
	}

	cmdline_paste_str(p, literally);
	if (allocated)
	    vim_free(arg);
	return OK;
    }

    return cmdline_paste_reg(regname, literally, remcr);
}