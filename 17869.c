tv_get_bool_or_number_chk(typval_T *varp, int *denote, int want_bool)
{
    varnumber_T	n = 0L;

    switch (varp->v_type)
    {
	case VAR_NUMBER:
	    if (in_vim9script() && want_bool && varp->vval.v_number != 0
						   && varp->vval.v_number != 1)
	    {
		semsg(_(e_using_number_as_bool_nr), varp->vval.v_number);
		break;
	    }
	    return varp->vval.v_number;
	case VAR_FLOAT:
#ifdef FEAT_FLOAT
	    emsg(_(e_using_float_as_number));
	    break;
#endif
	case VAR_FUNC:
	case VAR_PARTIAL:
	    emsg(_(e_using_funcref_as_number));
	    break;
	case VAR_STRING:
	    if (in_vim9script())
	    {
		emsg_using_string_as(varp, !want_bool);
		break;
	    }
	    if (varp->vval.v_string != NULL)
		vim_str2nr(varp->vval.v_string, NULL, NULL,
					    STR2NR_ALL, &n, NULL, 0, FALSE);
	    return n;
	case VAR_LIST:
	    emsg(_(e_using_list_as_number));
	    break;
	case VAR_DICT:
	    emsg(_(e_using_dictionary_as_number));
	    break;
	case VAR_BOOL:
	case VAR_SPECIAL:
	    if (!want_bool && in_vim9script())
	    {
		if (varp->v_type == VAR_BOOL)
		    emsg(_(e_using_bool_as_number));
		else
		    emsg(_(e_using_special_as_number));
		break;
	    }
	    return varp->vval.v_number == VVAL_TRUE ? 1 : 0;
	case VAR_JOB:
#ifdef FEAT_JOB_CHANNEL
	    emsg(_(e_using_job_as_number));
	    break;
#endif
	case VAR_CHANNEL:
#ifdef FEAT_JOB_CHANNEL
	    emsg(_(e_using_channel_as_number));
	    break;
#endif
	case VAR_BLOB:
	    emsg(_(e_using_blob_as_number));
	    break;
	case VAR_VOID:
	    emsg(_(e_cannot_use_void_value));
	    break;
	case VAR_UNKNOWN:
	case VAR_ANY:
	case VAR_INSTR:
	    internal_error_no_abort("tv_get_number(UNKNOWN)");
	    break;
    }
    if (denote == NULL)		// useful for values that must be unsigned
	n = -1;
    else
	*denote = TRUE;
    return n;
}