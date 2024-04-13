f_readdir(typval_T *argvars, typval_T *rettv)
{
    typval_T	*expr;
    int		ret;
    char_u	*path;
    char_u	*p;
    garray_T	ga;
    int		i;
    int         sort = READDIR_SORT_BYTE;

    if (rettv_list_alloc(rettv) == FAIL)
	return;

    if (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_dict_arg(argvars, 2) == FAIL)))
	return;

    path = tv_get_string(&argvars[0]);
    expr = &argvars[1];

    if (argvars[1].v_type != VAR_UNKNOWN && argvars[2].v_type != VAR_UNKNOWN &&
	    readdirex_dict_arg(&argvars[2], &sort) == FAIL)
	return;

    ret = readdir_core(&ga, path, FALSE, (void *)expr,
	    (expr->v_type == VAR_UNKNOWN) ? NULL : readdir_checkitem, sort);
    if (ret == OK)
    {
	for (i = 0; i < ga.ga_len; i++)
	{
	    p = ((char_u **)ga.ga_data)[i];
	    list_append_string(rettv->vval.v_list, p, -1);
	}
    }
    ga_clear_strings(&ga);
}