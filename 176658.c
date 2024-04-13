deref_function_name(
	    char_u	**arg,
	    char_u	**tofree,
	    evalarg_T	*evalarg,
	    int		verbose)
{
    typval_T	ref;
    char_u	*name = *arg;

    ref.v_type = VAR_UNKNOWN;
    if (eval7(arg, &ref, evalarg, FALSE) == FAIL)
    {
	dictitem_T	*v;

	// If <SID>VarName was used it would not be found, try another way.
	v = find_var_also_in_script(name, NULL, FALSE);
	if (v == NULL)
	    return NULL;
	copy_tv(&v->di_tv, &ref);
    }
    if (*skipwhite(*arg) != NUL)
    {
	if (verbose)
	    semsg(_(e_trailing_characters_str), *arg);
	name = NULL;
    }
    else if (ref.v_type == VAR_FUNC && ref.vval.v_string != NULL)
    {
	name = ref.vval.v_string;
	ref.vval.v_string = NULL;
	*tofree = name;
    }
    else if (ref.v_type == VAR_PARTIAL && ref.vval.v_partial != NULL)
    {
	if (ref.vval.v_partial->pt_argc > 0
		|| ref.vval.v_partial->pt_dict != NULL)
	{
	    if (verbose)
		emsg(_(e_cannot_use_partial_here));
	    name = NULL;
	}
	else
	{
	    name = vim_strsave(partial_name(ref.vval.v_partial));
	    *tofree = name;
	}
    }
    else
    {
	if (verbose)
	    semsg(_(e_not_callable_type_str), name);
	name = NULL;
    }
    clear_tv(&ref);
    return name;
}