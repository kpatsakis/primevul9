compile_load(
	char_u **arg,
	char_u *end_arg,
	cctx_T	*cctx,
	int	is_expr,
	int	error)
{
    type_T	*type;
    char_u	*name = NULL;
    char_u	*end = end_arg;
    int		res = FAIL;
    int		prev_called_emsg = called_emsg;

    if (*(*arg + 1) == ':')
    {
	if (end <= *arg + 2)
	{
	    isntype_T  isn_type;

	    // load dictionary of namespace
	    switch (**arg)
	    {
		case 'g': isn_type = ISN_LOADGDICT; break;
		case 'w': isn_type = ISN_LOADWDICT; break;
		case 't': isn_type = ISN_LOADTDICT; break;
		case 'b': isn_type = ISN_LOADBDICT; break;
		default:
		    semsg(_(e_namespace_not_supported_str), *arg);
		    goto theend;
	    }
	    if (generate_instr_type(cctx, isn_type, &t_dict_any) == NULL)
		goto theend;
	    res = OK;
	}
	else
	{
	    isntype_T  isn_type = ISN_DROP;

	    // load namespaced variable
	    name = vim_strnsave(*arg + 2, end - (*arg + 2));
	    if (name == NULL)
		return FAIL;

	    switch (**arg)
	    {
		case 'v': res = generate_LOADV(cctx, name, error);
			  break;
		case 's': if (is_expr && ASCII_ISUPPER(*name)
				       && find_func(name, FALSE, cctx) != NULL)
			      res = generate_funcref(cctx, name);
			  else
			      res = compile_load_scriptvar(cctx, name,
							    NULL, &end, error);
			  break;
		case 'g': if (vim_strchr(name, AUTOLOAD_CHAR) == NULL)
			  {
			      if (is_expr && ASCII_ISUPPER(*name)
				       && find_func(name, FALSE, cctx) != NULL)
				  res = generate_funcref(cctx, name);
			      else
				  isn_type = ISN_LOADG;
			  }
			  else
			  {
			      isn_type = ISN_LOADAUTO;
			      vim_free(name);
			      name = vim_strnsave(*arg, end - *arg);
			      if (name == NULL)
				  return FAIL;
			  }
			  break;
		case 'w': isn_type = ISN_LOADW; break;
		case 't': isn_type = ISN_LOADT; break;
		case 'b': isn_type = ISN_LOADB; break;
		default:  // cannot happen, just in case
			  semsg(_(e_namespace_not_supported_str), *arg);
			  goto theend;
	    }
	    if (isn_type != ISN_DROP)
	    {
		// Global, Buffer-local, Window-local and Tabpage-local
		// variables can be defined later, thus we don't check if it
		// exists, give an error at runtime.
		res = generate_LOAD(cctx, isn_type, 0, name, &t_any);
	    }
	}
    }
    else
    {
	size_t	    len = end - *arg;
	int	    idx;
	int	    gen_load = FALSE;
	int	    gen_load_outer = 0;

	name = vim_strnsave(*arg, end - *arg);
	if (name == NULL)
	    return FAIL;

	if (vim_strchr(name, AUTOLOAD_CHAR) != NULL)
	{
	    script_autoload(name, FALSE);
	    res = generate_LOAD(cctx, ISN_LOADAUTO, 0, name, &t_any);
	}
	else if (arg_exists(*arg, len, &idx, &type, &gen_load_outer, cctx)
									 == OK)
	{
	    if (gen_load_outer == 0)
		gen_load = TRUE;
	}
	else
	{
	    lvar_T lvar;

	    if (lookup_local(*arg, len, &lvar, cctx) == OK)
	    {
		type = lvar.lv_type;
		idx = lvar.lv_idx;
		if (lvar.lv_from_outer != 0)
		    gen_load_outer = lvar.lv_from_outer;
		else
		    gen_load = TRUE;
	    }
	    else
	    {
		// "var" can be script-local even without using "s:" if it
		// already exists in a Vim9 script or when it's imported.
		if (script_var_exists(*arg, len, cctx) == OK
			|| find_imported(name, 0, cctx) != NULL)
		   res = compile_load_scriptvar(cctx, name, *arg, &end, FALSE);

		// When evaluating an expression and the name starts with an
		// uppercase letter it can be a user defined function.
		// generate_funcref() will fail if the function can't be found.
		if (res == FAIL && is_expr && ASCII_ISUPPER(*name))
		    res = generate_funcref(cctx, name);
	    }
	}
	if (gen_load)
	    res = generate_LOAD(cctx, ISN_LOAD, idx, NULL, type);
	if (gen_load_outer > 0)
	{
	    res = generate_LOADOUTER(cctx, idx, gen_load_outer, type);
	    cctx->ctx_outer_used = TRUE;
	}
    }

    *arg = end;

theend:
    if (res == FAIL && error && called_emsg == prev_called_emsg)
	semsg(_(e_variable_not_found_str), name);
    vim_free(name);
    return res;
}