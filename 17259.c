f_getftype(typval_T *argvars, typval_T *rettv)
{
    char_u	*fname;
    stat_T	st;
    char_u	*type = NULL;

    if (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	return;

    fname = tv_get_string(&argvars[0]);

    rettv->v_type = VAR_STRING;
    if (mch_lstat((char *)fname, &st) >= 0)
	type = vim_strsave(getftypest(&st));
    rettv->vval.v_string = type;
}