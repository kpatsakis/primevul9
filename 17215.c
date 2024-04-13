f_glob(typval_T *argvars, typval_T *rettv)
{
    int		options = WILD_SILENT|WILD_USE_NL;
    expand_T	xpc;
    int		error = FALSE;

    if (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && (check_for_opt_bool_arg(argvars, 2) == FAIL
			|| (argvars[2].v_type != VAR_UNKNOWN
			    && check_for_opt_bool_arg(argvars, 3) == FAIL)))))
	return;

    // When the optional second argument is non-zero, don't remove matches
    // for 'wildignore' and don't put matches for 'suffixes' at the end.
    rettv->v_type = VAR_STRING;
    if (argvars[1].v_type != VAR_UNKNOWN)
    {
	if (tv_get_bool_chk(&argvars[1], &error))
	    options |= WILD_KEEP_ALL;
	if (argvars[2].v_type != VAR_UNKNOWN)
	{
	    if (tv_get_bool_chk(&argvars[2], &error))
		rettv_list_set(rettv, NULL);
	    if (argvars[3].v_type != VAR_UNKNOWN
				    && tv_get_bool_chk(&argvars[3], &error))
		options |= WILD_ALLLINKS;
	}
    }
    if (!error)
    {
	ExpandInit(&xpc);
	xpc.xp_context = EXPAND_FILES;
	if (p_wic)
	    options += WILD_ICASE;
	if (rettv->v_type == VAR_STRING)
	    rettv->vval.v_string = ExpandOne(&xpc, tv_get_string(&argvars[0]),
						     NULL, options, WILD_ALL);
	else if (rettv_list_alloc(rettv) != FAIL)
	{
	  int i;

	  ExpandOne(&xpc, tv_get_string(&argvars[0]),
						NULL, options, WILD_ALL_KEEP);
	  for (i = 0; i < xpc.xp_numfiles; i++)
	      list_append_string(rettv->vval.v_list, xpc.xp_files[i], -1);

	  ExpandCleanup(&xpc);
	}
    }
    else
	rettv->vval.v_string = NULL;
}