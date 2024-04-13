compile_redir(char_u *line, exarg_T *eap, cctx_T *cctx)
{
    char_u  *arg = eap->arg;
    lhs_T   *lhs = &cctx->ctx_redir_lhs;

    if (lhs->lhs_name != NULL)
    {
	if (STRNCMP(arg, "END", 3) == 0)
	{
	    if (lhs->lhs_append)
	    {
		// First load the current variable value.
		if (compile_load_lhs_with_index(lhs, lhs->lhs_whole,
								 cctx) == FAIL)
		    return NULL;
	    }

	    // Gets the redirected text and put it on the stack, then store it
	    // in the variable.
	    generate_instr_type(cctx, ISN_REDIREND, &t_string);

	    if (lhs->lhs_append)
		generate_CONCAT(cctx, 2);

	    if (lhs->lhs_has_index)
	    {
		// Use the info in "lhs" to store the value at the index in the
		// list or dict.
		if (compile_assign_unlet(lhs->lhs_whole, lhs, TRUE,
						      &t_string, cctx) == FAIL)
		    return NULL;
	    }
	    else if (generate_store_lhs(cctx, lhs, -1, FALSE) == FAIL)
		return NULL;

	    VIM_CLEAR(lhs->lhs_name);
	    VIM_CLEAR(lhs->lhs_whole);
	    return arg + 3;
	}
	emsg(_(e_cannot_nest_redir));
	return NULL;
    }

    if (arg[0] == '=' && arg[1] == '>')
    {
	int	    append = FALSE;

	// redirect to a variable is compiled
	arg += 2;
	if (*arg == '>')
	{
	    ++arg;
	    append = TRUE;
	}
	arg = skipwhite(arg);

	if (compile_assign_lhs(arg, lhs, CMD_redir,
					 FALSE, FALSE, FALSE, 1, cctx) == FAIL)
	    return NULL;
	if (need_type(&t_string, lhs->lhs_member_type,
					    -1, 0, cctx, FALSE, FALSE) == FAIL)
	    return NULL;
	generate_instr(cctx, ISN_REDIRSTART);
	lhs->lhs_append = append;
	if (lhs->lhs_has_index)
	{
	    lhs->lhs_whole = vim_strnsave(arg, lhs->lhs_varlen_total);
	    if (lhs->lhs_whole == NULL)
		return NULL;
	}

	return arg + lhs->lhs_varlen_total;
    }

    // other redirects are handled like at script level
    return compile_exec(line, eap, cctx);
}