eval3(char_u **arg, typval_T *rettv, evalarg_T *evalarg)
{
    char_u	*p;
    int		getnext;

    /*
     * Get the first variable.
     */
    if (eval4(arg, rettv, evalarg) == FAIL)
	return FAIL;

    /*
     * Handle the "&&" operator.
     */
    p = eval_next_non_blank(*arg, evalarg, &getnext);
    if (p[0] == '&' && p[1] == '&')
    {
	evalarg_T   *evalarg_used = evalarg;
	evalarg_T   local_evalarg;
	int	    orig_flags;
	int	    evaluate;
	long	    result = TRUE;
	typval_T    var2;
	int	    error = FALSE;
	int	    vim9script = in_vim9script();

	if (evalarg == NULL)
	{
	    init_evalarg(&local_evalarg);
	    evalarg_used = &local_evalarg;
	}
	orig_flags = evalarg_used->eval_flags;
	evaluate = orig_flags & EVAL_EVALUATE;
	if (evaluate)
	{
	    if (vim9script)
		result = tv_get_bool_chk(rettv, &error);
	    else if (tv_get_number_chk(rettv, &error) == 0)
		result = FALSE;
	    clear_tv(rettv);
	    if (error)
		return FAIL;
	}

	/*
	 * Repeat until there is no following "&&".
	 */
	while (p[0] == '&' && p[1] == '&')
	{
	    if (getnext)
		*arg = eval_next_line(evalarg_used);
	    else
	    {
		if (evaluate && vim9script && !VIM_ISWHITE(p[-1]))
		{
		    error_white_both(p, 2);
		    clear_tv(rettv);
		    return FAIL;
		}
		*arg = p;
	    }

	    /*
	     * Get the second variable.
	     */
	    if (evaluate && vim9script && !IS_WHITE_OR_NUL((*arg)[2]))
	    {
		error_white_both(*arg, 2);
		clear_tv(rettv);
		return FAIL;
	    }
	    *arg = skipwhite_and_linebreak(*arg + 2, evalarg_used);
	    evalarg_used->eval_flags = result ? orig_flags
						 : orig_flags & ~EVAL_EVALUATE;
	    CLEAR_FIELD(var2);
	    if (eval4(arg, &var2, evalarg_used) == FAIL)
		return FAIL;

	    /*
	     * Compute the result.
	     */
	    if (evaluate && result)
	    {
		if (vim9script)
		    result = tv_get_bool_chk(&var2, &error);
		else if (tv_get_number_chk(&var2, &error) == 0)
		    result = FALSE;
		clear_tv(&var2);
		if (error)
		    return FAIL;
	    }
	    if (evaluate)
	    {
		if (vim9script)
		{
		    rettv->v_type = VAR_BOOL;
		    rettv->vval.v_number = result ? VVAL_TRUE : VVAL_FALSE;
		}
		else
		{
		    rettv->v_type = VAR_NUMBER;
		    rettv->vval.v_number = result;
		}
	    }

	    p = eval_next_non_blank(*arg, evalarg_used, &getnext);
	}

	if (evalarg == NULL)
	    clear_evalarg(&local_evalarg, NULL);
	else
	    evalarg->eval_flags = orig_flags;
    }

    return OK;
}