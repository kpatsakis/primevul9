f_getcwd(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp = NULL;
    tabpage_T	*tp = NULL;
    char_u	*cwd;
    int		global = FALSE;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = NULL;

    if (in_vim9script()
	    && (check_for_opt_number_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type != VAR_UNKNOWN
		    && check_for_opt_number_arg(argvars, 1) == FAIL)))
	return;

    if (argvars[0].v_type == VAR_NUMBER
	    && argvars[0].vval.v_number == -1
	    && argvars[1].v_type == VAR_UNKNOWN)
	global = TRUE;
    else
	wp = find_tabwin(&argvars[0], &argvars[1], &tp);

    if (wp != NULL && wp->w_localdir != NULL
					   && argvars[0].v_type != VAR_UNKNOWN)
	rettv->vval.v_string = vim_strsave(wp->w_localdir);
    else if (tp != NULL && tp->tp_localdir != NULL
					   && argvars[0].v_type != VAR_UNKNOWN)
	rettv->vval.v_string = vim_strsave(tp->tp_localdir);
    else if (wp != NULL || tp != NULL || global)
    {
	if (globaldir != NULL && argvars[0].v_type != VAR_UNKNOWN)
	    rettv->vval.v_string = vim_strsave(globaldir);
	else
	{
	    cwd = alloc(MAXPATHL);
	    if (cwd != NULL)
	    {
		if (mch_dirname(cwd, MAXPATHL) != FAIL)
		    rettv->vval.v_string = vim_strsave(cwd);
		vim_free(cwd);
	    }
	}
    }
#ifdef BACKSLASH_IN_FILENAME
    if (rettv->vval.v_string != NULL)
	slash_adjust(rettv->vval.v_string);
#endif
}