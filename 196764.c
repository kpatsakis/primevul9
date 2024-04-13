compile_expr6(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    char_u	*op;
    char_u	*next;
    int		ppconst_used = ppconst->pp_used;

    // get the first expression
    if (compile_expr7(arg, cctx, ppconst) == FAIL)
	return FAIL;

    /*
     * Repeat computing, until no "*", "/" or "%" is following.
     */
    for (;;)
    {
	op = may_peek_next_line(cctx, *arg, &next);
	if (*op != '*' && *op != '/' && *op != '%')
	    break;
	if (next != NULL)
	{
	    *arg = next_line_from_context(cctx, TRUE);
	    op = skipwhite(*arg);
	}

	if (!IS_WHITE_OR_NUL(**arg) || !IS_WHITE_OR_NUL(op[1]))
	{
	    error_white_both(op, 1);
	    return FAIL;
	}
	if (may_get_next_line_error(op + 1, arg, cctx) == FAIL)
	    return FAIL;

	// get the second expression
	if (compile_expr7(arg, cctx, ppconst) == FAIL)
	    return FAIL;

	if (ppconst->pp_used == ppconst_used + 2
		&& ppconst->pp_tv[ppconst_used].v_type == VAR_NUMBER
		&& ppconst->pp_tv[ppconst_used + 1].v_type == VAR_NUMBER)
	{
	    typval_T	    *tv1 = &ppconst->pp_tv[ppconst_used];
	    typval_T	    *tv2 = &ppconst->pp_tv[ppconst_used + 1];
	    varnumber_T	    res = 0;
	    int		    failed = FALSE;

	    // both are numbers: compute the result
	    switch (*op)
	    {
		case '*': res = tv1->vval.v_number * tv2->vval.v_number;
			  break;
		case '/': res = num_divide(tv1->vval.v_number,
						  tv2->vval.v_number, &failed);
			  break;
		case '%': res = num_modulus(tv1->vval.v_number,
						  tv2->vval.v_number, &failed);
			  break;
	    }
	    if (failed)
		return FAIL;
	    tv1->vval.v_number = res;
	    --ppconst->pp_used;
	}
	else
	{
	    generate_ppconst(cctx, ppconst);
	    generate_two_op(cctx, op);
	}
    }

    return OK;
}