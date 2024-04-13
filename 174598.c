compile_substitute(char_u *arg, exarg_T *eap, cctx_T *cctx)
{
    char_u  *cmd = eap->arg;
    char_u  *expr = (char_u *)strstr((char *)cmd, "\\=");

    if (expr != NULL)
    {
	int delimiter = *cmd++;

	// There is a \=expr, find it in the substitute part.
	cmd = skip_regexp_ex(cmd, delimiter, magic_isset(), NULL, NULL, NULL);
	if (cmd[0] == delimiter && cmd[1] == '\\' && cmd[2] == '=')
	{
	    garray_T	save_ga = cctx->ctx_instr;
	    char_u	*end;
	    int		expr_res;
	    int		trailing_error;
	    int		instr_count;
	    isn_T	*instr;
	    isn_T	*isn;

	    cmd += 3;
	    end = skip_substitute(cmd, delimiter);

	    // Temporarily reset the list of instructions so that the jump
	    // labels are correct.
	    cctx->ctx_instr.ga_len = 0;
	    cctx->ctx_instr.ga_maxlen = 0;
	    cctx->ctx_instr.ga_data = NULL;
	    expr_res = compile_expr0(&cmd, cctx);
	    if (end[-1] == NUL)
		end[-1] = delimiter;
	    cmd = skipwhite(cmd);
	    trailing_error = *cmd != delimiter && *cmd != NUL;

	    if (expr_res == FAIL || trailing_error
				       || GA_GROW_FAILS(&cctx->ctx_instr, 1))
	    {
		if (trailing_error)
		    semsg(_(e_trailing_characters_str), cmd);
		clear_instr_ga(&cctx->ctx_instr);
		cctx->ctx_instr = save_ga;
		return NULL;
	    }

	    // Move the generated instructions into the ISN_SUBSTITUTE
	    // instructions, then restore the list of instructions before
	    // adding the ISN_SUBSTITUTE instruction.
	    instr_count = cctx->ctx_instr.ga_len;
	    instr = cctx->ctx_instr.ga_data;
	    instr[instr_count].isn_type = ISN_FINISH;

	    cctx->ctx_instr = save_ga;
	    if ((isn = generate_instr(cctx, ISN_SUBSTITUTE)) == NULL)
	    {
		int idx;

		for (idx = 0; idx < instr_count; ++idx)
		    delete_instr(instr + idx);
		vim_free(instr);
		return NULL;
	    }
	    isn->isn_arg.subs.subs_cmd = vim_strsave(arg);
	    isn->isn_arg.subs.subs_instr = instr;

	    // skip over flags
	    if (*end == '&')
		++end;
	    while (ASCII_ISALPHA(*end) || *end == '#')
		++end;
	    return end;
	}
    }

    return compile_exec(arg, eap, cctx);
}