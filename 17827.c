copy_tv(typval_T *from, typval_T *to)
{
    to->v_type = from->v_type;
    to->v_lock = 0;
    switch (from->v_type)
    {
	case VAR_NUMBER:
	case VAR_BOOL:
	case VAR_SPECIAL:
	    to->vval.v_number = from->vval.v_number;
	    break;
	case VAR_FLOAT:
#ifdef FEAT_FLOAT
	    to->vval.v_float = from->vval.v_float;
	    break;
#endif
	case VAR_JOB:
#ifdef FEAT_JOB_CHANNEL
	    to->vval.v_job = from->vval.v_job;
	    if (to->vval.v_job != NULL)
		++to->vval.v_job->jv_refcount;
	    break;
#endif
	case VAR_CHANNEL:
#ifdef FEAT_JOB_CHANNEL
	    to->vval.v_channel = from->vval.v_channel;
	    if (to->vval.v_channel != NULL)
		++to->vval.v_channel->ch_refcount;
	    break;
#endif
	case VAR_INSTR:
	    to->vval.v_instr = from->vval.v_instr;
	    break;

	case VAR_STRING:
	case VAR_FUNC:
	    if (from->vval.v_string == NULL)
		to->vval.v_string = NULL;
	    else
	    {
		to->vval.v_string = vim_strsave(from->vval.v_string);
		if (from->v_type == VAR_FUNC)
		    func_ref(to->vval.v_string);
	    }
	    break;
	case VAR_PARTIAL:
	    if (from->vval.v_partial == NULL)
		to->vval.v_partial = NULL;
	    else
	    {
		to->vval.v_partial = from->vval.v_partial;
		++to->vval.v_partial->pt_refcount;
	    }
	    break;
	case VAR_BLOB:
	    if (from->vval.v_blob == NULL)
		to->vval.v_blob = NULL;
	    else
	    {
		to->vval.v_blob = from->vval.v_blob;
		++to->vval.v_blob->bv_refcount;
	    }
	    break;
	case VAR_LIST:
	    if (from->vval.v_list == NULL)
		to->vval.v_list = NULL;
	    else
	    {
		to->vval.v_list = from->vval.v_list;
		++to->vval.v_list->lv_refcount;
	    }
	    break;
	case VAR_DICT:
	    if (from->vval.v_dict == NULL)
		to->vval.v_dict = NULL;
	    else
	    {
		to->vval.v_dict = from->vval.v_dict;
		++to->vval.v_dict->dv_refcount;
	    }
	    break;
	case VAR_VOID:
	    emsg(_(e_cannot_use_void_value));
	    break;
	case VAR_UNKNOWN:
	case VAR_ANY:
	    internal_error_no_abort("copy_tv(UNKNOWN)");
	    break;
    }
}