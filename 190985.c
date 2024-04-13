eval_for_line(
    char_u	*arg,
    int		*errp,
    exarg_T	*eap,
    evalarg_T	*evalarg)
{
    forinfo_T	*fi;
    char_u	*var_list_end;
    char_u	*expr;
    typval_T	tv;
    list_T	*l;
    int		skip = !(evalarg->eval_flags & EVAL_EVALUATE);

    *errp = TRUE;	// default: there is an error

    fi = ALLOC_CLEAR_ONE(forinfo_T);
    if (fi == NULL)
	return NULL;

    var_list_end = skip_var_list(arg, TRUE, &fi->fi_varcount,
						     &fi->fi_semicolon, FALSE);
    if (var_list_end == NULL)
	return fi;

    expr = skipwhite_and_linebreak(var_list_end, evalarg);
    if (expr[0] != 'i' || expr[1] != 'n'
				  || !(expr[2] == NUL || VIM_ISWHITE(expr[2])))
    {
	if (in_vim9script() && *expr == ':' && expr != var_list_end)
	    semsg(_(e_no_white_space_allowed_before_colon_str), expr);
	else
	    emsg(_(e_missing_in));
	return fi;
    }

    if (skip)
	++emsg_skip;
    expr = skipwhite_and_linebreak(expr + 2, evalarg);
    if (eval0(expr, &tv, eap, evalarg) == OK)
    {
	*errp = FALSE;
	if (!skip)
	{
	    if (tv.v_type == VAR_LIST)
	    {
		l = tv.vval.v_list;
		if (l == NULL)
		{
		    // a null list is like an empty list: do nothing
		    clear_tv(&tv);
		}
		else
		{
		    // Need a real list here.
		    CHECK_LIST_MATERIALIZE(l);

		    // No need to increment the refcount, it's already set for
		    // the list being used in "tv".
		    fi->fi_list = l;
		    list_add_watch(l, &fi->fi_lw);
		    fi->fi_lw.lw_item = l->lv_first;
		}
	    }
	    else if (tv.v_type == VAR_BLOB)
	    {
		fi->fi_bi = 0;
		if (tv.vval.v_blob != NULL)
		{
		    typval_T btv;

		    // Make a copy, so that the iteration still works when the
		    // blob is changed.
		    blob_copy(tv.vval.v_blob, &btv);
		    fi->fi_blob = btv.vval.v_blob;
		}
		clear_tv(&tv);
	    }
	    else if (tv.v_type == VAR_STRING)
	    {
		fi->fi_byte_idx = 0;
		fi->fi_string = tv.vval.v_string;
		tv.vval.v_string = NULL;
		if (fi->fi_string == NULL)
		    fi->fi_string = vim_strsave((char_u *)"");
	    }
	    else
	    {
		emsg(_(e_string_list_or_blob_required));
		clear_tv(&tv);
	    }
	}
    }
    if (skip)
	--emsg_skip;
    fi->fi_break_count = evalarg->eval_break_count;

    return fi;
}