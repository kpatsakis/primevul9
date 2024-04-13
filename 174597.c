compile_put(char_u *arg, exarg_T *eap, cctx_T *cctx)
{
    char_u	*line = arg;
    linenr_T	lnum;
    char	*errormsg;
    int		above = eap->forceit;

    eap->regname = *line;

    if (eap->regname == '=')
    {
	char_u *p = skipwhite(line + 1);

	if (compile_expr0(&p, cctx) == FAIL)
	    return NULL;
	line = p;
    }
    else if (eap->regname != NUL)
	++line;

    if (compile_variable_range(eap, cctx) == OK)
    {
	lnum = above ? LNUM_VARIABLE_RANGE_ABOVE : LNUM_VARIABLE_RANGE;
    }
    else
    {
	// Either no range or a number.
	// "errormsg" will not be set because the range is ADDR_LINES.
	if (parse_cmd_address(eap, &errormsg, FALSE) == FAIL)
	    // cannot happen
	    return NULL;
	if (eap->addr_count == 0)
	    lnum = -1;
	else
	    lnum = eap->line2;
	if (above)
	    --lnum;
    }

    generate_PUT(cctx, eap->regname, lnum);
    return line;
}