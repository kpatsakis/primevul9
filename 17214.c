f_globpath(typval_T *argvars, typval_T *rettv)
{
    int		flags = WILD_IGNORE_COMPLETESLASH;
    char_u	buf1[NUMBUFLEN];
    char_u	*file;
    int		error = FALSE;
    garray_T	ga;
    int		i;

    if (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_bool_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && (check_for_opt_bool_arg(argvars, 3) == FAIL
			|| (argvars[3].v_type != VAR_UNKNOWN
			    && check_for_opt_bool_arg(argvars, 4) == FAIL)))))
	return;

    file = tv_get_string_buf_chk(&argvars[1], buf1);

    // When the optional second argument is non-zero, don't remove matches
    // for 'wildignore' and don't put matches for 'suffixes' at the end.
    rettv->v_type = VAR_STRING;
    if (argvars[2].v_type != VAR_UNKNOWN)
    {
	if (tv_get_bool_chk(&argvars[2], &error))
	    flags |= WILD_KEEP_ALL;
	if (argvars[3].v_type != VAR_UNKNOWN)
	{
	    if (tv_get_bool_chk(&argvars[3], &error))
		rettv_list_set(rettv, NULL);
	    if (argvars[4].v_type != VAR_UNKNOWN
				    && tv_get_bool_chk(&argvars[4], &error))
		flags |= WILD_ALLLINKS;
	}
    }
    if (file != NULL && !error)
    {
	ga_init2(&ga, sizeof(char_u *), 10);
	globpath(tv_get_string(&argvars[0]), file, &ga, flags);
	if (rettv->v_type == VAR_STRING)
	    rettv->vval.v_string = ga_concat_strings(&ga, "\n");
	else if (rettv_list_alloc(rettv) != FAIL)
	    for (i = 0; i < ga.ga_len; ++i)
		list_append_string(rettv->vval.v_list,
					    ((char_u **)(ga.ga_data))[i], -1);
	ga_clear_strings(&ga);
    }
    else
	rettv->vval.v_string = NULL;
}