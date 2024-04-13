eval_index(
    char_u	**arg,
    typval_T	*rettv,
    evalarg_T	*evalarg,
    int		verbose)	// give error messages
{
    int		evaluate = evalarg != NULL
				      && (evalarg->eval_flags & EVAL_EVALUATE);
    int		empty1 = FALSE, empty2 = FALSE;
    typval_T	var1, var2;
    int		range = FALSE;
    char_u	*key = NULL;
    int		keylen = -1;
    int		vim9 = in_vim9script();

    if (check_can_index(rettv, evaluate, verbose) == FAIL)
	return FAIL;

    init_tv(&var1);
    init_tv(&var2);
    if (**arg == '.')
    {
	/*
	 * dict.name
	 */
	key = *arg + 1;
	for (keylen = 0; eval_isdictc(key[keylen]); ++keylen)
	    ;
	if (keylen == 0)
	    return FAIL;
	*arg = key + keylen;
    }
    else
    {
	/*
	 * something[idx]
	 *
	 * Get the (first) variable from inside the [].
	 */
	*arg = skipwhite_and_linebreak(*arg + 1, evalarg);
	if (**arg == ':')
	    empty1 = TRUE;
	else if (eval1(arg, &var1, evalarg) == FAIL)	// recursive!
	    return FAIL;
	else if (vim9 && **arg == ':')
	{
	    semsg(_(e_white_space_required_before_and_after_str_at_str),
								    ":", *arg);
	    clear_tv(&var1);
	    return FAIL;
	}
	else if (evaluate)
	{
#ifdef FEAT_FLOAT
	    // allow for indexing with float
	    if (vim9 && rettv->v_type == VAR_DICT
						   && var1.v_type == VAR_FLOAT)
	    {
		var1.vval.v_string = typval_tostring(&var1, TRUE);
		var1.v_type = VAR_STRING;
	    }
#endif
	    if (tv_get_string_chk(&var1) == NULL)
	    {
		// not a number or string
		clear_tv(&var1);
		return FAIL;
	    }
	}

	/*
	 * Get the second variable from inside the [:].
	 */
	*arg = skipwhite_and_linebreak(*arg, evalarg);
	if (**arg == ':')
	{
	    range = TRUE;
	    ++*arg;
	    if (vim9 && !IS_WHITE_OR_NUL(**arg) && **arg != ']')
	    {
		semsg(_(e_white_space_required_before_and_after_str_at_str),
								":", *arg - 1);
		if (!empty1)
		    clear_tv(&var1);
		return FAIL;
	    }
	    *arg = skipwhite_and_linebreak(*arg, evalarg);
	    if (**arg == ']')
		empty2 = TRUE;
	    else if (eval1(arg, &var2, evalarg) == FAIL)	// recursive!
	    {
		if (!empty1)
		    clear_tv(&var1);
		return FAIL;
	    }
	    else if (evaluate && tv_get_string_chk(&var2) == NULL)
	    {
		// not a number or string
		if (!empty1)
		    clear_tv(&var1);
		clear_tv(&var2);
		return FAIL;
	    }
	}

	// Check for the ']'.
	*arg = skipwhite_and_linebreak(*arg, evalarg);
	if (**arg != ']')
	{
	    if (verbose)
		emsg(_(e_missing_closing_square_brace));
	    clear_tv(&var1);
	    if (range)
		clear_tv(&var2);
	    return FAIL;
	}
	*arg = *arg + 1;	// skip over the ']'
    }

    if (evaluate)
    {
	int res = eval_index_inner(rettv, range,
		empty1 ? NULL : &var1, empty2 ? NULL : &var2, FALSE,
		key, keylen, verbose);

	if (!empty1)
	    clear_tv(&var1);
	if (range)
	    clear_tv(&var2);
	return res;
    }
    return OK;
}