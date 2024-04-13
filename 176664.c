eval4(char_u **arg, typval_T *rettv, evalarg_T *evalarg)
{
    char_u	*p;
    int		getnext;
    exprtype_T	type = EXPR_UNKNOWN;
    int		len = 2;
    int		type_is = FALSE;

    /*
     * Get the first variable.
     */
    if (eval5(arg, rettv, evalarg) == FAIL)
	return FAIL;

    p = eval_next_non_blank(*arg, evalarg, &getnext);
    type = get_compare_type(p, &len, &type_is);

    /*
     * If there is a comparative operator, use it.
     */
    if (type != EXPR_UNKNOWN)
    {
	typval_T    var2;
	int	    ic;
	int	    vim9script = in_vim9script();
	int	    evaluate = evalarg == NULL
				   ? 0 : (evalarg->eval_flags & EVAL_EVALUATE);
	long	    comp_lnum = SOURCING_LNUM;

	if (getnext)
	{
	    *arg = eval_next_line(evalarg);
	    p = *arg;
	}
	else if (evaluate && vim9script && !VIM_ISWHITE(**arg))
	{
	    error_white_both(*arg, len);
	    clear_tv(rettv);
	    return FAIL;
	}

	if (vim9script && type_is && (p[len] == '?' || p[len] == '#'))
	{
	    semsg(_(e_invalid_expression_str), p);
	    clear_tv(rettv);
	    return FAIL;
	}

	// extra question mark appended: ignore case
	if (p[len] == '?')
	{
	    ic = TRUE;
	    ++len;
	}
	// extra '#' appended: match case
	else if (p[len] == '#')
	{
	    ic = FALSE;
	    ++len;
	}
	// nothing appended: use 'ignorecase' if not in Vim script
	else
	    ic = vim9script ? FALSE : p_ic;

	/*
	 * Get the second variable.
	 */
	if (evaluate && vim9script && !IS_WHITE_OR_NUL(p[len]))
	{
	    error_white_both(p, len);
	    clear_tv(rettv);
	    return FAIL;
	}
	*arg = skipwhite_and_linebreak(p + len, evalarg);
	if (eval5(arg, &var2, evalarg) == FAIL)
	{
	    clear_tv(rettv);
	    return FAIL;
	}
	if (evaluate)
	{
	    int ret;

	    // use the line of the comparison for messages
	    SOURCING_LNUM = comp_lnum;
	    if (vim9script && check_compare_types(type, rettv, &var2) == FAIL)
	    {
		ret = FAIL;
		clear_tv(rettv);
	    }
	    else
		ret = typval_compare(rettv, &var2, type, ic);
	    clear_tv(&var2);
	    return ret;
	}
    }

    return OK;
}