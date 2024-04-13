compile_expr5(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    char_u	*op;
    char_u	*next;
    int		oplen;
    int		ppconst_used = ppconst->pp_used;

    // get the first variable
    if (compile_expr6(arg, cctx, ppconst) == FAIL)
	return FAIL;

    /*
     * Repeat computing, until no "+", "-" or ".." is following.
     */
    for (;;)
    {
	op = may_peek_next_line(cctx, *arg, &next);
	if (*op != '+' && *op != '-' && !(*op == '.' && *(op + 1) == '.'))
	    break;
	if (op[0] == op[1] && *op != '.' && next)
	    // Finding "++" or "--" on the next line is a separate command.
	    // But ".." is concatenation.
	    break;
	oplen = (*op == '.' ? 2 : 1);
	if (next != NULL)
	{
	    *arg = next_line_from_context(cctx, TRUE);
	    op = skipwhite(*arg);
	}

	if (!IS_WHITE_OR_NUL(**arg) || !IS_WHITE_OR_NUL(op[oplen]))
	{
	    error_white_both(op, oplen);
	    return FAIL;
	}

	if (may_get_next_line_error(op + oplen, arg, cctx) == FAIL)
	    return FAIL;

	// get the second expression
	if (compile_expr6(arg, cctx, ppconst) == FAIL)
	    return FAIL;

	if (ppconst->pp_used == ppconst_used + 2
		&& (*op == '.'
		    ? (ppconst->pp_tv[ppconst_used].v_type == VAR_STRING
		    && ppconst->pp_tv[ppconst_used + 1].v_type == VAR_STRING)
		    : (ppconst->pp_tv[ppconst_used].v_type == VAR_NUMBER
		    && ppconst->pp_tv[ppconst_used + 1].v_type == VAR_NUMBER)))
	{
	    typval_T *tv1 = &ppconst->pp_tv[ppconst_used];
	    typval_T *tv2 = &ppconst->pp_tv[ppconst_used + 1];

	    // concat/subtract/add constant numbers
	    if (*op == '+')
		tv1->vval.v_number = tv1->vval.v_number + tv2->vval.v_number;
	    else if (*op == '-')
		tv1->vval.v_number = tv1->vval.v_number - tv2->vval.v_number;
	    else
	    {
		// concatenate constant strings
		char_u *s1 = tv1->vval.v_string;
		char_u *s2 = tv2->vval.v_string;
		size_t len1 = STRLEN(s1);

		tv1->vval.v_string = alloc((int)(len1 + STRLEN(s2) + 1));
		if (tv1->vval.v_string == NULL)
		{
		    clear_ppconst(ppconst);
		    return FAIL;
		}
		mch_memmove(tv1->vval.v_string, s1, len1);
		STRCPY(tv1->vval.v_string + len1, s2);
		vim_free(s1);
		vim_free(s2);
	    }
	    --ppconst->pp_used;
	}
	else
	{
	    generate_ppconst(cctx, ppconst);
	    ppconst->pp_is_const = FALSE;
	    if (*op == '.')
	    {
		if (may_generate_2STRING(-2, FALSE, cctx) == FAIL
			|| may_generate_2STRING(-1, FALSE, cctx) == FAIL)
		    return FAIL;
		generate_instr_drop(cctx, ISN_CONCAT, 1);
	    }
	    else
		generate_two_op(cctx, op);
	}
    }

    return OK;
}