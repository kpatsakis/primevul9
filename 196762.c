compile_parenthesis(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    int	    ret;
    char_u  *p = *arg + 1;

    if (may_get_next_line_error(p, arg, cctx) == FAIL)
	return FAIL;
    if (ppconst->pp_used <= PPSIZE - 10)
    {
	ret = compile_expr1(arg, cctx, ppconst);
    }
    else
    {
	// Not enough space in ppconst, flush constants.
	if (generate_ppconst(cctx, ppconst) == FAIL)
	    return FAIL;
	ret = compile_expr0(arg, cctx);
    }
    if (may_get_next_line_error(*arg, arg, cctx) == FAIL)
	return FAIL;
    if (**arg == ')')
	++*arg;
    else if (ret == OK)
    {
	emsg(_(e_missing_closing_paren));
	ret = FAIL;
    }
    return ret;
}