f_getfperm(typval_T *argvars, typval_T *rettv)
{
    char_u	*fname;
    stat_T	st;
    char_u	*perm = NULL;
    char_u	permbuf[] = "---------";

    if (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	return;

    fname = tv_get_string(&argvars[0]);

    rettv->v_type = VAR_STRING;
    if (mch_stat((char *)fname, &st) >= 0)
	perm = vim_strsave(getfpermst(&st, permbuf));
    rettv->vval.v_string = perm;
}