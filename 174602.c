compile_exec(char_u *line_arg, exarg_T *eap, cctx_T *cctx)
{
    char_u	*line = line_arg;
    char_u	*p;
    int		has_expr = FALSE;
    char_u	*nextcmd = (char_u *)"";
    char_u	*tofree = NULL;
    char_u	*cmd_arg = NULL;

    if (cctx->ctx_skip == SKIP_YES)
	goto theend;

    // If there was a prececing command modifier, drop it and include it in the
    // EXEC command.
    if (cctx->ctx_has_cmdmod)
    {
	garray_T	*instr = &cctx->ctx_instr;
	isn_T		*isn = ((isn_T *)instr->ga_data) + instr->ga_len - 1;

	if (isn->isn_type == ISN_CMDMOD)
	{
	    vim_regfree(isn->isn_arg.cmdmod.cf_cmdmod
					       ->cmod_filter_regmatch.regprog);
	    vim_free(isn->isn_arg.cmdmod.cf_cmdmod);
	    --instr->ga_len;
	    cctx->ctx_has_cmdmod = FALSE;
	}
    }

    if (eap->cmdidx >= 0 && eap->cmdidx < CMD_SIZE)
    {
	long	argt = eap->argt;
	int	usefilter = FALSE;

	has_expr = argt & (EX_XFILE | EX_EXPAND);

	// If the command can be followed by a bar, find the bar and truncate
	// it, so that the following command can be compiled.
	// The '|' is overwritten with a NUL, it is put back below.
	if ((eap->cmdidx == CMD_write || eap->cmdidx == CMD_read)
							   && *eap->arg == '!')
	    // :w !filter or :r !filter or :r! filter
	    usefilter = TRUE;
	if ((argt & EX_TRLBAR) && !usefilter)
	{
	    eap->argt = argt;
	    separate_nextcmd(eap, TRUE);
	    if (eap->nextcmd != NULL)
		nextcmd = eap->nextcmd;
	}
	else if (eap->cmdidx == CMD_wincmd)
	{
	    p = eap->arg;
	    if (*p != NUL)
		++p;
	    if (*p == 'g' || *p == Ctrl_G)
		++p;
	    p = skipwhite(p);
	    if (*p == '|')
	    {
		*p = NUL;
		nextcmd = p + 1;
	    }
	}
	else if (eap->cmdidx == CMD_command || eap->cmdidx == CMD_autocmd)
	{
	    // If there is a trailing '{' read lines until the '}'
	    p = eap->arg + STRLEN(eap->arg) - 1;
	    while (p > eap->arg && VIM_ISWHITE(*p))
		--p;
	    if (*p == '{')
	    {
		exarg_T ea;
		int	flags = 0;  // unused
		int	start_lnum = SOURCING_LNUM;

		CLEAR_FIELD(ea);
		ea.arg = eap->arg;
		fill_exarg_from_cctx(&ea, cctx);
		(void)may_get_cmd_block(&ea, p, &tofree, &flags);
		if (tofree != NULL)
		{
		    *p = NUL;
		    line = concat_str(line, tofree);
		    if (line == NULL)
			goto theend;
		    vim_free(tofree);
		    tofree = line;
		    SOURCING_LNUM = start_lnum;
		}
	    }
	}
    }

    if (eap->cmdidx == CMD_syntax && STRNCMP(eap->arg, "include ", 8) == 0)
    {
	// expand filename in "syntax include [@group] filename"
	has_expr = TRUE;
	eap->arg = skipwhite(eap->arg + 7);
	if (*eap->arg == '@')
	    eap->arg = skiptowhite(eap->arg);
    }

    if ((eap->cmdidx == CMD_global || eap->cmdidx == CMD_vglobal)
						       && STRLEN(eap->arg) > 4)
    {
	int delim = *eap->arg;

	p = skip_regexp_ex(eap->arg + 1, delim, TRUE, NULL, NULL, NULL);
	if (*p == delim)
	    cmd_arg = p + 1;
    }

    if (eap->cmdidx == CMD_folddoopen || eap->cmdidx == CMD_folddoclosed)
	cmd_arg = eap->arg;

    if (cmd_arg != NULL)
    {
	exarg_T nea;

	CLEAR_FIELD(nea);
	nea.cmd = cmd_arg;
	p = find_ex_command(&nea, NULL, lookup_scriptitem, NULL);
	if (nea.cmdidx < CMD_SIZE)
	{
	    has_expr = excmd_get_argt(nea.cmdidx) & (EX_XFILE | EX_EXPAND);
	    if (has_expr)
		eap->arg = skiptowhite(eap->arg);
	}
    }

    if (has_expr && (p = (char_u *)strstr((char *)eap->arg, "`=")) != NULL)
    {
	int	count = 0;
	char_u	*start = skipwhite(line);

	// :cmd xxx`=expr1`yyy`=expr2`zzz
	// PUSHS ":cmd xxx"
	// eval expr1
	// PUSHS "yyy"
	// eval expr2
	// PUSHS "zzz"
	// EXECCONCAT 5
	for (;;)
	{
	    if (p > start)
	    {
		char_u *val = vim_strnsave(start, p - start);

		generate_PUSHS(cctx, &val);
		++count;
	    }
	    p += 2;
	    if (compile_expr0(&p, cctx) == FAIL)
		return NULL;
	    may_generate_2STRING(-1, TRUE, cctx);
	    ++count;
	    p = skipwhite(p);
	    if (*p != '`')
	    {
		emsg(_(e_missing_backtick));
		return NULL;
	    }
	    start = p + 1;

	    p = (char_u *)strstr((char *)start, "`=");
	    if (p == NULL)
	    {
		if (*skipwhite(start) != NUL)
		{
		    char_u *val = vim_strsave(start);

		    generate_PUSHS(cctx, &val);
		    ++count;
		}
		break;
	    }
	}
	generate_EXECCONCAT(cctx, count);
    }
    else
	generate_EXEC_copy(cctx, ISN_EXEC, line);

theend:
    if (*nextcmd != NUL)
    {
	// the parser expects a pointer to the bar, put it back
	--nextcmd;
	*nextcmd = '|';
    }
    vim_free(tofree);

    return nextcmd;
}