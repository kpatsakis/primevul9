f_setcmdline(typval_T *argvars, typval_T *rettv)
{
    int pos = -1;

    if (check_for_string_arg(argvars, 0) == FAIL
	    || check_for_opt_number_arg(argvars, 1) == FAIL)
	return;

    if (argvars[1].v_type != VAR_UNKNOWN)
    {
	int error = FALSE;

	pos = (int)tv_get_number_chk(&argvars[1], &error) - 1;
	if (error)
	    return;
	if (pos < 0)
	{
	    emsg(_(e_argument_must_be_positive));
	    return;
	}
    }

    rettv->vval.v_number = set_cmdline_str(argvars[0].vval.v_string, pos);
}