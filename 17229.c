f_readfile(typval_T *argvars, typval_T *rettv)
{
    if (in_vim9script()
	    && (check_for_nonempty_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_number_arg(argvars, 2) == FAIL)))
	return;

    read_file_or_blob(argvars, rettv, FALSE);
}