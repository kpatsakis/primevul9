eval7t(
    char_u	**arg,
    typval_T	*rettv,
    evalarg_T	*evalarg,
    int		want_string)	// after "." operator
{
    type_T	*want_type = NULL;
    garray_T	type_list;	    // list of pointers to allocated types
    int		res;
    int		evaluate = evalarg == NULL ? 0
				       : (evalarg->eval_flags & EVAL_EVALUATE);

    // Recognize <type> in Vim9 script only.
    if (in_vim9script() && **arg == '<' && eval_isnamec1((*arg)[1])
					     && STRNCMP(*arg, "<SNR>", 5) != 0)
    {
	++*arg;
	ga_init2(&type_list, sizeof(type_T *), 10);
	want_type = parse_type(arg, &type_list, TRUE);
	if (want_type == NULL && (evaluate || **arg != '>'))
	{
	    clear_type_list(&type_list);
	    return FAIL;
	}

	if (**arg != '>')
	{
	    if (*skipwhite(*arg) == '>')
		semsg(_(e_no_white_space_allowed_before_str_str), ">", *arg);
	    else
		emsg(_(e_missing_gt));
	    clear_type_list(&type_list);
	    return FAIL;
	}
	++*arg;
	*arg = skipwhite_and_linebreak(*arg, evalarg);
    }

    res = eval7(arg, rettv, evalarg, want_string);

    if (want_type != NULL && evaluate)
    {
	if (res == OK)
	{
	    type_T *actual = typval2type(rettv, get_copyID(), &type_list, TRUE);

	    if (!equal_type(want_type, actual, 0))
	    {
		if (want_type == &t_bool && actual != &t_bool
					&& (actual->tt_flags & TTFLAG_BOOL_OK))
		{
		    int n = tv2bool(rettv);

		    // can use "0" and "1" for boolean in some places
		    clear_tv(rettv);
		    rettv->v_type = VAR_BOOL;
		    rettv->vval.v_number = n ? VVAL_TRUE : VVAL_FALSE;
		}
		else
		{
		    where_T where = WHERE_INIT;

		    where.wt_variable = TRUE;
		    res = check_type(want_type, actual, TRUE, where);
		}
	    }
	}
	clear_type_list(&type_list);
    }

    return res;
}