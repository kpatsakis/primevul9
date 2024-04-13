f_mkdir(typval_T *argvars, typval_T *rettv)
{
    char_u	*dir;
    char_u	buf[NUMBUFLEN];
    int		prot = 0755;

    rettv->vval.v_number = FAIL;
    if (check_restricted() || check_secure())
	return;

    if (in_vim9script()
	    && (check_for_nonempty_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_number_arg(argvars, 2) == FAIL)))
	return;

    dir = tv_get_string_buf(&argvars[0], buf);
    if (*dir == NUL)
	return;

    if (*gettail(dir) == NUL)
	// remove trailing slashes
	*gettail_sep(dir) = NUL;

    if (argvars[1].v_type != VAR_UNKNOWN)
    {
	if (argvars[2].v_type != VAR_UNKNOWN)
	{
	    prot = (int)tv_get_number_chk(&argvars[2], NULL);
	    if (prot == -1)
		return;
	}
	if (STRCMP(tv_get_string(&argvars[1]), "p") == 0)
	{
	    if (mch_isdir(dir))
	    {
		// With the "p" flag it's OK if the dir already exists.
		rettv->vval.v_number = OK;
		return;
	    }
	    mkdir_recurse(dir, prot);
	}
    }
    rettv->vval.v_number = vim_mkdir_emsg(dir, prot);
}