eval_method(
    char_u	**arg,
    typval_T	*rettv,
    evalarg_T	*evalarg,
    int		verbose)	// give error messages
{
    char_u	*name;
    long	len;
    char_u	*alias;
    typval_T	base = *rettv;
    int		ret;
    int		evaluate = evalarg != NULL
				      && (evalarg->eval_flags & EVAL_EVALUATE);

    rettv->v_type = VAR_UNKNOWN;

    name = *arg;
    len = get_name_len(arg, &alias, evaluate, TRUE);
    if (alias != NULL)
	name = alias;

    if (len <= 0)
    {
	if (verbose)
	    emsg(_("E260: Missing name after ->"));
	ret = FAIL;
    }
    else
    {
	*arg = skipwhite(*arg);
	if (**arg != '(')
	{
	    if (verbose)
		semsg(_(e_missing_parenthesis_str), name);
	    ret = FAIL;
	}
	else if (VIM_ISWHITE((*arg)[-1]))
	{
	    if (verbose)
		emsg(_(e_nowhitespace));
	    ret = FAIL;
	}
	else
	    ret = eval_func(arg, evalarg, name, len, rettv,
					  evaluate ? EVAL_EVALUATE : 0, &base);
    }

    // Clear the funcref afterwards, so that deleting it while
    // evaluating the arguments is possible (see test55).
    if (evaluate)
	clear_tv(&base);

    return ret;
}