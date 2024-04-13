compile_expr4(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    exprtype_T	type = EXPR_UNKNOWN;
    char_u	*p;
    char_u	*next;
    int		len = 2;
    int		type_is = FALSE;
    int		ppconst_used = ppconst->pp_used;

    // get the first variable
    if (compile_expr5(arg, cctx, ppconst) == FAIL)
	return FAIL;

    p = may_peek_next_line(cctx, *arg, &next);
    type = get_compare_type(p, &len, &type_is);

    /*
     * If there is a comparative operator, use it.
     */
    if (type != EXPR_UNKNOWN)
    {
	int ic = FALSE;  // Default: do not ignore case

	if (next != NULL)
	{
	    *arg = next_line_from_context(cctx, TRUE);
	    p = skipwhite(*arg);
	}
	if (type_is && (p[len] == '?' || p[len] == '#'))
	{
	    semsg(_(e_invalid_expression_str), *arg);
	    return FAIL;
	}
	// extra question mark appended: ignore case
	if (p[len] == '?')
	{
	    ic = TRUE;
	    ++len;
	}
	// extra '#' appended: match case (ignored)
	else if (p[len] == '#')
	    ++len;
	// nothing appended: match case

	if (!IS_WHITE_OR_NUL(**arg) || !IS_WHITE_OR_NUL(p[len]))
	{
	    error_white_both(p, len);
	    return FAIL;
	}

	// get the second variable
	if (may_get_next_line_error(p + len, arg, cctx) == FAIL)
	    return FAIL;

	if (compile_expr5(arg, cctx, ppconst) == FAIL)
	    return FAIL;

	if (ppconst->pp_used == ppconst_used + 2)
	{
	    typval_T *	tv1 = &ppconst->pp_tv[ppconst->pp_used - 2];
	    typval_T	*tv2 = &ppconst->pp_tv[ppconst->pp_used - 1];
	    int		ret;

	    // Both sides are a constant, compute the result now.
	    // First check for a valid combination of types, this is more
	    // strict than typval_compare().
	    if (check_compare_types(type, tv1, tv2) == FAIL)
		ret = FAIL;
	    else
	    {
		ret = typval_compare(tv1, tv2, type, ic);
		tv1->v_type = VAR_BOOL;
		tv1->vval.v_number = tv1->vval.v_number
						      ? VVAL_TRUE : VVAL_FALSE;
		clear_tv(tv2);
		--ppconst->pp_used;
	    }
	    return ret;
	}

	generate_ppconst(cctx, ppconst);
	return generate_COMPARE(cctx, type, ic);
    }

    return OK;
}