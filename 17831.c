eval_option(
    char_u	**arg,
    typval_T	*rettv,	// when NULL, only check if option exists
    int		evaluate)
{
    char_u	*option_end;
    long	numval;
    char_u	*stringval;
    getoption_T	opt_type;
    int		c;
    int		working = (**arg == '+');    // has("+option")
    int		ret = OK;
    int		scope;

    // Isolate the option name and find its value.
    option_end = find_option_end(arg, &scope);
    if (option_end == NULL)
    {
	if (rettv != NULL)
	    semsg(_(e_option_name_missing_str), *arg);
	return FAIL;
    }

    if (!evaluate)
    {
	*arg = option_end;
	return OK;
    }

    c = *option_end;
    *option_end = NUL;
    opt_type = get_option_value(*arg, &numval,
			       rettv == NULL ? NULL : &stringval, NULL, scope);

    if (opt_type == gov_unknown)
    {
	if (rettv != NULL)
	    semsg(_(e_unknown_option_str), *arg);
	ret = FAIL;
    }
    else if (rettv != NULL)
    {
	rettv->v_lock = 0;
	if (opt_type == gov_hidden_string)
	{
	    rettv->v_type = VAR_STRING;
	    rettv->vval.v_string = NULL;
	}
	else if (opt_type == gov_hidden_bool || opt_type == gov_hidden_number)
	{
	    rettv->v_type = in_vim9script() && opt_type == gov_hidden_bool
						       ? VAR_BOOL : VAR_NUMBER;
	    rettv->vval.v_number = 0;
	}
	else if (opt_type == gov_bool || opt_type == gov_number)
	{
	    if (in_vim9script() && opt_type == gov_bool)
	    {
		rettv->v_type = VAR_BOOL;
		rettv->vval.v_number = numval ? VVAL_TRUE : VVAL_FALSE;
	    }
	    else
	    {
		rettv->v_type = VAR_NUMBER;
		rettv->vval.v_number = numval;
	    }
	}
	else				// string option
	{
	    rettv->v_type = VAR_STRING;
	    rettv->vval.v_string = stringval;
	}
    }
    else if (working && (opt_type == gov_hidden_bool
			|| opt_type == gov_hidden_number
			|| opt_type == gov_hidden_string))
	ret = FAIL;

    *option_end = c;		    // put back for error messages
    *arg = option_end;

    return ret;
}