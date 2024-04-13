f_test_getvalue(typval_T *argvars, typval_T *rettv)
{
    char_u *name;

    if (check_for_string_arg(argvars, 0) == FAIL)
	return;

    name = tv_get_string(&argvars[0]);

    if (STRCMP(name, (char_u *)"need_fileinfo") == 0)
	rettv->vval.v_number = need_fileinfo;
    else
	semsg(_(e_invalid_argument_str), name);
}