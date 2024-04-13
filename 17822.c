tv_get_float_chk(typval_T *varp, int *error)
{
    switch (varp->v_type)
    {
	case VAR_NUMBER:
	    return (float_T)(varp->vval.v_number);
	case VAR_FLOAT:
	    return varp->vval.v_float;
	case VAR_FUNC:
	case VAR_PARTIAL:
	    emsg(_(e_using_funcref_as_float));
	    break;
	case VAR_STRING:
	    emsg(_(e_using_string_as_float));
	    break;
	case VAR_LIST:
	    emsg(_(e_using_list_as_float));
	    break;
	case VAR_DICT:
	    emsg(_(e_using_dictionary_as_float));
	    break;
	case VAR_BOOL:
	    emsg(_(e_using_boolean_value_as_float));
	    break;
	case VAR_SPECIAL:
	    emsg(_(e_using_special_value_as_float));
	    break;
	case VAR_JOB:
# ifdef FEAT_JOB_CHANNEL
	    emsg(_(e_using_job_as_float));
	    break;
# endif
	case VAR_CHANNEL:
# ifdef FEAT_JOB_CHANNEL
	    emsg(_(e_using_channel_as_float));
	    break;
# endif
	case VAR_BLOB:
	    emsg(_(e_using_blob_as_float));
	    break;
	case VAR_VOID:
	    emsg(_(e_cannot_use_void_value));
	    break;
	case VAR_UNKNOWN:
	case VAR_ANY:
	case VAR_INSTR:
	    internal_error_no_abort("tv_get_float(UNKNOWN)");
	    break;
    }
    if (error != NULL)
	*error = TRUE;
    return 0;
}