f_test_ignore_error(typval_T *argvars, typval_T *rettv UNUSED)
{
    if (check_for_string_arg(argvars, 0) == FAIL)
	return;

    ignore_error_for_testing(tv_get_string(&argvars[0]));
}