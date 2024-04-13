f_delete(typval_T *argvars, typval_T *rettv)
{
    char_u	nbuf[NUMBUFLEN];
    char_u	*name;
    char_u	*flags;

    rettv->vval.v_number = -1;
    if (check_restricted() || check_secure())
	return;

    if (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	return;

    name = tv_get_string(&argvars[0]);
    if (name == NULL || *name == NUL)
    {
	emsg(_(e_invalid_argument));
	return;
    }

    if (argvars[1].v_type != VAR_UNKNOWN)
	flags = tv_get_string_buf(&argvars[1], nbuf);
    else
	flags = (char_u *)"";

    if (*flags == NUL)
	// delete a file
	rettv->vval.v_number = mch_remove(name) == 0 ? 0 : -1;
    else if (STRCMP(flags, "d") == 0)
	// delete an empty directory
	rettv->vval.v_number = mch_rmdir(name) == 0 ? 0 : -1;
    else if (STRCMP(flags, "rf") == 0)
	// delete a directory recursively
	rettv->vval.v_number = delete_recursive(name);
    else
	semsg(_(e_invalid_expression_str), flags);
}