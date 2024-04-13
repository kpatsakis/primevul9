f_exepath(typval_T *argvars, typval_T *rettv)
{
    char_u *p = NULL;

    if (in_vim9script() && check_for_nonempty_string_arg(argvars, 0) == FAIL)
	return;
    (void)mch_can_exe(tv_get_string(&argvars[0]), &p, TRUE);
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = p;
}