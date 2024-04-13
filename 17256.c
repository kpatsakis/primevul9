f_filewritable(typval_T *argvars, typval_T *rettv)
{
    if (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	return;
    rettv->vval.v_number = filewritable(tv_get_string(&argvars[0]));
}