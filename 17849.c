clear_tv(typval_T *varp)
{
    if (varp != NULL)
    {
	switch (varp->v_type)
	{
	    case VAR_FUNC:
		func_unref(varp->vval.v_string);
		// FALLTHROUGH
	    case VAR_STRING:
		VIM_CLEAR(varp->vval.v_string);
		break;
	    case VAR_PARTIAL:
		partial_unref(varp->vval.v_partial);
		varp->vval.v_partial = NULL;
		break;
	    case VAR_BLOB:
		blob_unref(varp->vval.v_blob);
		varp->vval.v_blob = NULL;
		break;
	    case VAR_LIST:
		list_unref(varp->vval.v_list);
		varp->vval.v_list = NULL;
		break;
	    case VAR_DICT:
		dict_unref(varp->vval.v_dict);
		varp->vval.v_dict = NULL;
		break;
	    case VAR_NUMBER:
	    case VAR_BOOL:
	    case VAR_SPECIAL:
		varp->vval.v_number = 0;
		break;
	    case VAR_FLOAT:
#ifdef FEAT_FLOAT
		varp->vval.v_float = 0.0;
		break;
#endif
	    case VAR_JOB:
#ifdef FEAT_JOB_CHANNEL
		job_unref(varp->vval.v_job);
		varp->vval.v_job = NULL;
#endif
		break;
	    case VAR_CHANNEL:
#ifdef FEAT_JOB_CHANNEL
		channel_unref(varp->vval.v_channel);
		varp->vval.v_channel = NULL;
#endif
		break;
	    case VAR_INSTR:
		VIM_CLEAR(varp->vval.v_instr);
		break;
	    case VAR_UNKNOWN:
	    case VAR_ANY:
	    case VAR_VOID:
		break;
	}
	varp->v_lock = 0;
    }
}