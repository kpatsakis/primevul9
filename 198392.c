get_func_tv(
    char_u	*name,		// name of the function
    int		len,		// length of "name" or -1 to use strlen()
    typval_T	*rettv,
    char_u	**arg,		// argument, pointing to the '('
    evalarg_T	*evalarg,	// for line continuation
    funcexe_T	*funcexe)	// various values
{
    char_u	*argp;
    int		ret = OK;
    typval_T	argvars[MAX_FUNC_ARGS + 1];	// vars for arguments
    int		argcount = 0;		// number of arguments found
    int		vim9script = in_vim9script();

    /*
     * Get the arguments.
     */
    argp = *arg;
    while (argcount < MAX_FUNC_ARGS - (funcexe->fe_partial == NULL ? 0
					       : funcexe->fe_partial->pt_argc))
    {
	// skip the '(' or ',' and possibly line breaks
	argp = skipwhite_and_linebreak(argp + 1, evalarg);

	if (*argp == ')' || *argp == ',' || *argp == NUL)
	    break;
	if (eval1(&argp, &argvars[argcount], evalarg) == FAIL)
	{
	    ret = FAIL;
	    break;
	}
	++argcount;
	// The comma should come right after the argument, but this wasn't
	// checked previously, thus only enforce it in Vim9 script.
	if (vim9script)
	{
	    if (*argp != ',' && *skipwhite(argp) == ',')
	    {
		semsg(_(e_no_white_space_allowed_before_str_str), ",", argp);
		ret = FAIL;
		break;
	    }
	}
	else
	    argp = skipwhite(argp);
	if (*argp != ',')
	    break;
	if (vim9script && !IS_WHITE_OR_NUL(argp[1]))
	{
	    semsg(_(e_white_space_required_after_str_str), ",", argp);
	    ret = FAIL;
	    break;
	}
    }
    argp = skipwhite_and_linebreak(argp, evalarg);
    if (*argp == ')')
	++argp;
    else
	ret = FAIL;

    if (ret == OK)
    {
	int	i = 0;
	int	did_emsg_before = did_emsg;

	if (get_vim_var_nr(VV_TESTING))
	{
	    // Prepare for calling test_garbagecollect_now(), need to know
	    // what variables are used on the call stack.
	    if (funcargs.ga_itemsize == 0)
		ga_init2(&funcargs, (int)sizeof(typval_T *), 50);
	    for (i = 0; i < argcount; ++i)
		if (ga_grow(&funcargs, 1) == OK)
		    ((typval_T **)funcargs.ga_data)[funcargs.ga_len++] =
								  &argvars[i];
	}

	ret = call_func(name, len, rettv, argcount, argvars, funcexe);
	if (in_vim9script() && did_emsg > did_emsg_before)
	{
	    // An error in a builtin function does not return FAIL, but we do
	    // want to abort further processing if an error was given.
	    ret = FAIL;
	    clear_tv(rettv);
	}

	funcargs.ga_len -= i;
    }
    else if (!aborting())
    {
	if (argcount == MAX_FUNC_ARGS)
	    emsg_funcname(N_("E740: Too many arguments for function %s"), name);
	else
	    emsg_funcname(e_invalid_arguments_for_function_str, name);
    }

    while (--argcount >= 0)
	clear_tv(&argvars[argcount]);

    if (in_vim9script())
	*arg = argp;
    else
	*arg = skipwhite(argp);
    return ret;
}