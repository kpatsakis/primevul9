eval_func(
	char_u	    **arg,	// points to "(", will be advanced
	evalarg_T   *evalarg,
	char_u	    *name,
	int	    name_len,
	typval_T    *rettv,
	int	    flags,
	typval_T    *basetv)	// "expr" for "expr->name(arg)"
{
    int		evaluate = flags & EVAL_EVALUATE;
    char_u	*s = name;
    int		len = name_len;
    partial_T	*partial;
    int		ret = OK;
    type_T	*type = NULL;
    int		found_var = FALSE;

    if (!evaluate)
	check_vars(s, len);

    // If "s" is the name of a variable of type VAR_FUNC
    // use its contents.
    s = deref_func_name(s, &len, &partial,
			in_vim9script() ? &type : NULL, !evaluate, &found_var);

    // Need to make a copy, in case evaluating the arguments makes
    // the name invalid.
    s = vim_strsave(s);
    if (s == NULL || (evaluate && (*s == NUL || (flags & EVAL_CONSTANT))))
	ret = FAIL;
    else
    {
	funcexe_T funcexe;

	// Invoke the function.
	CLEAR_FIELD(funcexe);
	funcexe.fe_firstline = curwin->w_cursor.lnum;
	funcexe.fe_lastline = curwin->w_cursor.lnum;
	funcexe.fe_evaluate = evaluate;
	funcexe.fe_partial = partial;
	funcexe.fe_basetv = basetv;
	funcexe.fe_check_type = type;
	funcexe.fe_found_var = found_var;
	ret = get_func_tv(s, len, rettv, arg, evalarg, &funcexe);
    }
    vim_free(s);

    // If evaluate is FALSE rettv->v_type was not set in
    // get_func_tv, but it's needed in handle_subscript() to parse
    // what follows. So set it here.
    if (rettv->v_type == VAR_UNKNOWN && !evaluate && **arg == '(')
    {
	rettv->vval.v_string = NULL;
	rettv->v_type = VAR_FUNC;
    }

    // Stop the expression evaluation when immediately
    // aborting on error, or when an interrupt occurred or
    // an exception was thrown but not caught.
    if (evaluate && aborting())
    {
	if (ret == OK)
	    clear_tv(rettv);
	ret = FAIL;
    }
    return ret;
}