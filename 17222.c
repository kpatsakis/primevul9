f_pathshorten(typval_T *argvars, typval_T *rettv)
{
    char_u	*p;
    int		trim_len = 1;

    if (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_number_arg(argvars, 1) == FAIL))
	return;

    if (argvars[1].v_type != VAR_UNKNOWN)
    {
	trim_len = (int)tv_get_number(&argvars[1]);
	if (trim_len < 1)
	    trim_len = 1;
    }

    rettv->v_type = VAR_STRING;
    p = tv_get_string_chk(&argvars[0]);

    if (p == NULL)
	rettv->vval.v_string = NULL;
    else
    {
	p = vim_strsave(p);
	rettv->vval.v_string = p;
	if (p != NULL)
	    shorten_dir_len(p, trim_len);
    }
}