eval6(
    char_u	**arg,
    typval_T	*rettv,
    evalarg_T	*evalarg,
    int		want_string)  // after "." operator
{
#ifdef FEAT_FLOAT
    int	    use_float = FALSE;
#endif

    /*
     * Get the first variable.
     */
    if (eval7t(arg, rettv, evalarg, want_string) == FAIL)
	return FAIL;

    /*
     * Repeat computing, until no '*', '/' or '%' is following.
     */
    for (;;)
    {
	int	    evaluate;
	int	    getnext;
	typval_T    var2;
	char_u	    *p;
	int	    op;
	varnumber_T n1, n2;
#ifdef FEAT_FLOAT
	float_T	    f1, f2;
#endif
	int	    error;

	// "*=", "/=" and "%=" are assignments
	p = eval_next_non_blank(*arg, evalarg, &getnext);
	op = *p;
	if ((op != '*' && op != '/' && op != '%') || p[1] == '=')
	    break;

	evaluate = evalarg == NULL ? 0 : (evalarg->eval_flags & EVAL_EVALUATE);
	if (getnext)
	    *arg = eval_next_line(evalarg);
	else
	{
	    if (evaluate && in_vim9script() && !VIM_ISWHITE(**arg))
	    {
		error_white_both(*arg, 1);
		clear_tv(rettv);
		return FAIL;
	    }
	    *arg = p;
	}

#ifdef FEAT_FLOAT
	f1 = 0;
	f2 = 0;
#endif
	error = FALSE;
	if (evaluate)
	{
#ifdef FEAT_FLOAT
	    if (rettv->v_type == VAR_FLOAT)
	    {
		f1 = rettv->vval.v_float;
		use_float = TRUE;
		n1 = 0;
	    }
	    else
#endif
		n1 = tv_get_number_chk(rettv, &error);
	    clear_tv(rettv);
	    if (error)
		return FAIL;
	}
	else
	    n1 = 0;

	/*
	 * Get the second variable.
	 */
	if (evaluate && in_vim9script() && !IS_WHITE_OR_NUL((*arg)[1]))
	{
	    error_white_both(*arg, 1);
	    clear_tv(rettv);
	    return FAIL;
	}
	*arg = skipwhite_and_linebreak(*arg + 1, evalarg);
	if (eval7t(arg, &var2, evalarg, FALSE) == FAIL)
	    return FAIL;

	if (evaluate)
	{
#ifdef FEAT_FLOAT
	    if (var2.v_type == VAR_FLOAT)
	    {
		if (!use_float)
		{
		    f1 = n1;
		    use_float = TRUE;
		}
		f2 = var2.vval.v_float;
		n2 = 0;
	    }
	    else
#endif
	    {
		n2 = tv_get_number_chk(&var2, &error);
		clear_tv(&var2);
		if (error)
		    return FAIL;
#ifdef FEAT_FLOAT
		if (use_float)
		    f2 = n2;
#endif
	    }

	    /*
	     * Compute the result.
	     * When either side is a float the result is a float.
	     */
#ifdef FEAT_FLOAT
	    if (use_float)
	    {
		if (op == '*')
		    f1 = f1 * f2;
		else if (op == '/')
		{
# ifdef VMS
		    // VMS crashes on divide by zero, work around it
		    if (f2 == 0.0)
		    {
			if (f1 == 0)
			    f1 = -1 * __F_FLT_MAX - 1L;   // similar to NaN
			else if (f1 < 0)
			    f1 = -1 * __F_FLT_MAX;
			else
			    f1 = __F_FLT_MAX;
		    }
		    else
			f1 = f1 / f2;
# else
		    // We rely on the floating point library to handle divide
		    // by zero to result in "inf" and not a crash.
		    f1 = f1 / f2;
# endif
		}
		else
		{
		    emsg(_(e_modulus));
		    return FAIL;
		}
		rettv->v_type = VAR_FLOAT;
		rettv->vval.v_float = f1;
	    }
	    else
#endif
	    {
		int	    failed = FALSE;

		if (op == '*')
		    n1 = n1 * n2;
		else if (op == '/')
		    n1 = num_divide(n1, n2, &failed);
		else
		    n1 = num_modulus(n1, n2, &failed);
		if (failed)
		    return FAIL;

		rettv->v_type = VAR_NUMBER;
		rettv->vval.v_number = n1;
	    }
	}
    }

    return OK;
}