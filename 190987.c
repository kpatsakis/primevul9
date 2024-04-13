set_var_lval(
    lval_T	*lp,
    char_u	*endp,
    typval_T	*rettv,
    int		copy,
    int		flags,	    // ASSIGN_CONST, ASSIGN_NO_DECL
    char_u	*op,
    int		var_idx)    // index for "let [a, b] = list"
{
    int		cc;
    dictitem_T	*di;

    if (lp->ll_tv == NULL)
    {
	cc = *endp;
	*endp = NUL;
	if (in_vim9script() && check_reserved_name(lp->ll_name) == FAIL)
	    return;

	if (lp->ll_blob != NULL)
	{
	    int	    error = FALSE, val;

	    if (op != NULL && *op != '=')
	    {
		semsg(_(e_letwrong), op);
		return;
	    }
	    if (value_check_lock(lp->ll_blob->bv_lock, lp->ll_name, FALSE))
		return;

	    if (lp->ll_range && rettv->v_type == VAR_BLOB)
	    {
		if (lp->ll_empty2)
		    lp->ll_n2 = blob_len(lp->ll_blob) - 1;

		if (blob_set_range(lp->ll_blob, lp->ll_n1, lp->ll_n2,
								rettv) == FAIL)
		    return;
	    }
	    else
	    {
		val = (int)tv_get_number_chk(rettv, &error);
		if (!error)
		    blob_set_append(lp->ll_blob, lp->ll_n1, val);
	    }
	}
	else if (op != NULL && *op != '=')
	{
	    typval_T tv;

	    if ((flags & (ASSIGN_CONST | ASSIGN_FINAL))
					     && (flags & ASSIGN_FOR_LOOP) == 0)
	    {
		emsg(_(e_cannot_mod));
		*endp = cc;
		return;
	    }

	    // handle +=, -=, *=, /=, %= and .=
	    di = NULL;
	    if (eval_variable(lp->ll_name, (int)STRLEN(lp->ll_name),
					     &tv, &di, EVAL_VAR_VERBOSE) == OK)
	    {
		if ((di == NULL
			 || (!var_check_ro(di->di_flags, lp->ll_name, FALSE)
			   && !tv_check_lock(&di->di_tv, lp->ll_name, FALSE)))
			&& tv_op(&tv, rettv, op) == OK)
		    set_var_const(lp->ll_name, NULL, &tv, FALSE,
							    ASSIGN_NO_DECL, 0);
		clear_tv(&tv);
	    }
	}
	else
	{
	    if (lp->ll_type != NULL && check_typval_arg_type(lp->ll_type, rettv,
							      NULL, 0) == FAIL)
		return;
	    set_var_const(lp->ll_name, lp->ll_type, rettv, copy,
							       flags, var_idx);
	}
	*endp = cc;
    }
    else if (value_check_lock(lp->ll_newkey == NULL
		? lp->ll_tv->v_lock
		: lp->ll_tv->vval.v_dict->dv_lock, lp->ll_name, FALSE))
	;
    else if (lp->ll_range)
    {
	if ((flags & (ASSIGN_CONST | ASSIGN_FINAL))
					     && (flags & ASSIGN_FOR_LOOP) == 0)
	{
	    emsg(_("E996: Cannot lock a range"));
	    return;
	}

	(void)list_assign_range(lp->ll_list, rettv->vval.v_list,
			 lp->ll_n1, lp->ll_n2, lp->ll_empty2, op, lp->ll_name);
    }
    else
    {
	/*
	 * Assign to a List or Dictionary item.
	 */
	if ((flags & (ASSIGN_CONST | ASSIGN_FINAL))
					     && (flags & ASSIGN_FOR_LOOP) == 0)
	{
	    emsg(_("E996: Cannot lock a list or dict"));
	    return;
	}

	if (lp->ll_valtype != NULL
		    && check_typval_arg_type(lp->ll_valtype, rettv,
							      NULL, 0) == FAIL)
	    return;

	if (lp->ll_newkey != NULL)
	{
	    if (op != NULL && *op != '=')
	    {
		semsg(_(e_dictkey), lp->ll_newkey);
		return;
	    }
	    if (dict_wrong_func_name(lp->ll_tv->vval.v_dict, rettv,
								lp->ll_newkey))
		return;

	    // Need to add an item to the Dictionary.
	    di = dictitem_alloc(lp->ll_newkey);
	    if (di == NULL)
		return;
	    if (dict_add(lp->ll_tv->vval.v_dict, di) == FAIL)
	    {
		vim_free(di);
		return;
	    }
	    lp->ll_tv = &di->di_tv;
	}
	else if (op != NULL && *op != '=')
	{
	    tv_op(lp->ll_tv, rettv, op);
	    return;
	}
	else
	    clear_tv(lp->ll_tv);

	/*
	 * Assign the value to the variable or list item.
	 */
	if (copy)
	    copy_tv(rettv, lp->ll_tv);
	else
	{
	    *lp->ll_tv = *rettv;
	    lp->ll_tv->v_lock = 0;
	    init_tv(rettv);
	}
    }
}