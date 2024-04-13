one_function_arg(
	char_u	    *arg,
	garray_T    *newargs,
	garray_T    *argtypes,
	int	    types_optional,
	evalarg_T   *evalarg,
	int	    is_vararg,
	int	    skip)
{
    char_u	*p = arg;
    char_u	*arg_copy = NULL;
    int		is_underscore = FALSE;

    while (ASCII_ISALNUM(*p) || *p == '_')
	++p;
    if (arg == p || isdigit(*arg)
	    || (argtypes == NULL
		&& ((p - arg == 9 && STRNCMP(arg, "firstline", 9) == 0)
		    || (p - arg == 8 && STRNCMP(arg, "lastline", 8) == 0))))
    {
	if (!skip)
	    semsg(_(e_illegal_argument_str), arg);
	return arg;
    }

    // Vim9 script: cannot use script var name for argument. In function: also
    // check local vars and arguments.
    if (!skip && argtypes != NULL && check_defined(arg, p - arg,
		    evalarg == NULL ? NULL : evalarg->eval_cctx, TRUE) == FAIL)
	return arg;

    if (newargs != NULL && ga_grow(newargs, 1) == FAIL)
	return arg;
    if (newargs != NULL)
    {
	int	c;
	int	i;

	c = *p;
	*p = NUL;
	arg_copy = vim_strsave(arg);
	if (arg_copy == NULL)
	{
	    *p = c;
	    return arg;
	}
	is_underscore = arg_copy[0] == '_' && arg_copy[1] == NUL;
	if (argtypes == NULL || !is_underscore)
	    // Check for duplicate argument name.
	    for (i = 0; i < newargs->ga_len; ++i)
		if (STRCMP(((char_u **)(newargs->ga_data))[i], arg_copy) == 0)
		{
		    semsg(_("E853: Duplicate argument name: %s"), arg_copy);
		    vim_free(arg_copy);
		    return arg;
		}
	((char_u **)(newargs->ga_data))[newargs->ga_len] = arg_copy;
	newargs->ga_len++;

	*p = c;
    }

    // get any type from "arg: type"
    if (argtypes != NULL && (skip || ga_grow(argtypes, 1) == OK))
    {
	char_u *type = NULL;

	if (VIM_ISWHITE(*p) && *skipwhite(p) == ':')
	{
	    semsg(_(e_no_white_space_allowed_before_colon_str),
					    arg_copy == NULL ? arg : arg_copy);
	    p = skipwhite(p);
	}
	if (*p == ':')
	{
	    ++p;
	    if (!skip && !VIM_ISWHITE(*p))
	    {
		semsg(_(e_white_space_required_after_str_str), ":", p - 1);
		return arg;
	    }
	    type = skipwhite(p);
	    p = skip_type(type, TRUE);
	    if (!skip)
		type = vim_strnsave(type, p - type);
	}
	else if (*skipwhite(p) != '=' && !types_optional && !is_underscore)
	{
	    semsg(_(e_missing_argument_type_for_str),
					    arg_copy == NULL ? arg : arg_copy);
	    return arg;
	}
	if (!skip)
	{
	    if (type == NULL && types_optional)
		// lambda arguments default to "any" type
		type = vim_strsave((char_u *)
					    (is_vararg ? "list<any>" : "any"));
	    ((char_u **)argtypes->ga_data)[argtypes->ga_len++] = type;
	}
    }

    return p;
}