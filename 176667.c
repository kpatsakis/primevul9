echo_string_core(
    typval_T	*tv,
    char_u	**tofree,
    char_u	*numbuf,
    int		copyID,
    int		echo_style,
    int		restore_copyID,
    int		composite_val)
{
    static int	recurse = 0;
    char_u	*r = NULL;

    if (recurse >= DICT_MAXNEST)
    {
	if (!did_echo_string_emsg)
	{
	    // Only give this message once for a recursive call to avoid
	    // flooding the user with errors.  And stop iterating over lists
	    // and dicts.
	    did_echo_string_emsg = TRUE;
	    emsg(_(e_variable_nested_too_deep_for_displaying));
	}
	*tofree = NULL;
	return (char_u *)"{E724}";
    }
    ++recurse;

    switch (tv->v_type)
    {
	case VAR_STRING:
	    if (echo_style && !composite_val)
	    {
		*tofree = NULL;
		r = tv->vval.v_string;
		if (r == NULL)
		    r = (char_u *)"";
	    }
	    else
	    {
		*tofree = string_quote(tv->vval.v_string, FALSE);
		r = *tofree;
	    }
	    break;

	case VAR_FUNC:
	    {
		char_u buf[MAX_FUNC_NAME_LEN];

		if (echo_style)
		{
		    r = make_ufunc_name_readable(tv->vval.v_string,
						       buf, MAX_FUNC_NAME_LEN);
		    if (r == buf)
		    {
			r = vim_strsave(buf);
			*tofree = r;
		    }
		    else
			*tofree = NULL;
		}
		else
		{
		    *tofree = string_quote(tv->vval.v_string == NULL ? NULL
			    : make_ufunc_name_readable(
				tv->vval.v_string, buf, MAX_FUNC_NAME_LEN),
									 TRUE);
		    r = *tofree;
		}
	    }
	    break;

	case VAR_PARTIAL:
	    {
		partial_T   *pt = tv->vval.v_partial;
		char_u	    *fname = string_quote(pt == NULL ? NULL
						    : partial_name(pt), FALSE);
		garray_T    ga;
		int	    i;
		char_u	    *tf;

		ga_init2(&ga, 1, 100);
		ga_concat(&ga, (char_u *)"function(");
		if (fname != NULL)
		{
		    // When using uf_name prepend "g:" for a global function.
		    if (pt != NULL && pt->pt_name == NULL && fname[0] == '\''
						      && vim_isupper(fname[1]))
		    {
			ga_concat(&ga, (char_u *)"'g:");
			ga_concat(&ga, fname + 1);
		    }
		    else
			ga_concat(&ga, fname);
		    vim_free(fname);
		}
		if (pt != NULL && pt->pt_argc > 0)
		{
		    ga_concat(&ga, (char_u *)", [");
		    for (i = 0; i < pt->pt_argc; ++i)
		    {
			if (i > 0)
			    ga_concat(&ga, (char_u *)", ");
			ga_concat(&ga,
			     tv2string(&pt->pt_argv[i], &tf, numbuf, copyID));
			vim_free(tf);
		    }
		    ga_concat(&ga, (char_u *)"]");
		}
		if (pt != NULL && pt->pt_dict != NULL)
		{
		    typval_T dtv;

		    ga_concat(&ga, (char_u *)", ");
		    dtv.v_type = VAR_DICT;
		    dtv.vval.v_dict = pt->pt_dict;
		    ga_concat(&ga, tv2string(&dtv, &tf, numbuf, copyID));
		    vim_free(tf);
		}
		// terminate with ')' and a NUL
		ga_concat_len(&ga, (char_u *)")", 2);

		*tofree = ga.ga_data;
		r = *tofree;
		break;
	    }

	case VAR_BLOB:
	    r = blob2string(tv->vval.v_blob, tofree, numbuf);
	    break;

	case VAR_LIST:
	    if (tv->vval.v_list == NULL)
	    {
		// NULL list is equivalent to empty list.
		*tofree = NULL;
		r = (char_u *)"[]";
	    }
	    else if (copyID != 0 && tv->vval.v_list->lv_copyID == copyID
		    && tv->vval.v_list->lv_len > 0)
	    {
		*tofree = NULL;
		r = (char_u *)"[...]";
	    }
	    else
	    {
		int old_copyID = tv->vval.v_list->lv_copyID;

		tv->vval.v_list->lv_copyID = copyID;
		*tofree = list2string(tv, copyID, restore_copyID);
		if (restore_copyID)
		    tv->vval.v_list->lv_copyID = old_copyID;
		r = *tofree;
	    }
	    break;

	case VAR_DICT:
	    if (tv->vval.v_dict == NULL)
	    {
		// NULL dict is equivalent to empty dict.
		*tofree = NULL;
		r = (char_u *)"{}";
	    }
	    else if (copyID != 0 && tv->vval.v_dict->dv_copyID == copyID
		    && tv->vval.v_dict->dv_hashtab.ht_used != 0)
	    {
		*tofree = NULL;
		r = (char_u *)"{...}";
	    }
	    else
	    {
		int old_copyID = tv->vval.v_dict->dv_copyID;

		tv->vval.v_dict->dv_copyID = copyID;
		*tofree = dict2string(tv, copyID, restore_copyID);
		if (restore_copyID)
		    tv->vval.v_dict->dv_copyID = old_copyID;
		r = *tofree;
	    }
	    break;

	case VAR_NUMBER:
	case VAR_UNKNOWN:
	case VAR_ANY:
	case VAR_VOID:
	    *tofree = NULL;
	    r = tv_get_string_buf(tv, numbuf);
	    break;

	case VAR_JOB:
	case VAR_CHANNEL:
#ifdef FEAT_JOB_CHANNEL
	    *tofree = NULL;
	    r = tv->v_type == VAR_JOB ? job_to_string_buf(tv, numbuf)
					   : channel_to_string_buf(tv, numbuf);
	    if (composite_val)
	    {
		*tofree = string_quote(r, FALSE);
		r = *tofree;
	    }
#endif
	    break;

	case VAR_INSTR:
	    *tofree = NULL;
	    r = (char_u *)"instructions";
	    break;

	case VAR_FLOAT:
#ifdef FEAT_FLOAT
	    *tofree = NULL;
	    vim_snprintf((char *)numbuf, NUMBUFLEN, "%g", tv->vval.v_float);
	    r = numbuf;
	    break;
#endif

	case VAR_BOOL:
	case VAR_SPECIAL:
	    *tofree = NULL;
	    r = (char_u *)get_var_special_name(tv->vval.v_number);
	    break;
    }

    if (--recurse == 0)
	did_echo_string_emsg = FALSE;
    return r;
}