checkitem_common(void *context, char_u *name, dict_T *dict)
{
    typval_T	*expr = (typval_T *)context;
    typval_T	save_val;
    typval_T	rettv;
    typval_T	argv[2];
    int		retval = 0;
    int		error = FALSE;

    prepare_vimvar(VV_VAL, &save_val);
    if (name != NULL)
    {
	set_vim_var_string(VV_VAL, name, -1);
	argv[0].v_type = VAR_STRING;
	argv[0].vval.v_string = name;
    }
    else
    {
	set_vim_var_dict(VV_VAL, dict);
	argv[0].v_type = VAR_DICT;
	argv[0].vval.v_dict = dict;
    }

    if (eval_expr_typval(expr, argv, 1, &rettv) == FAIL)
	goto theend;

    // We want to use -1, but also true/false should be allowed.
    if (rettv.v_type == VAR_SPECIAL || rettv.v_type == VAR_BOOL)
    {
	rettv.v_type = VAR_NUMBER;
	rettv.vval.v_number = rettv.vval.v_number == VVAL_TRUE;
    }
    retval = tv_get_number_chk(&rettv, &error);
    if (error)
	retval = -1;
    clear_tv(&rettv);

theend:
    if (name != NULL)
	set_vim_var_string(VV_VAL, NULL, 0);
    else
	set_vim_var_dict(VV_VAL, NULL);
    restore_vimvar(VV_VAL, &save_val);
    return retval;
}