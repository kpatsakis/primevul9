free_tv(typval_T *varp)
{
    if (varp != NULL)
    {
	switch (varp->v_type)
	{
	    case VAR_FUNC:
		func_unref(varp->vval.v_string);
		// FALLTHROUGH
	    case VAR_STRING:
		vim_free(varp->vval.v_string);
		break;
	    case VAR_PARTIAL:
		partial_unref(varp->vval.v_partial);
		break;
	    case VAR_BLOB:
		blob_unref(varp->vval.v_blob);
		break;
	    case VAR_LIST:
		list_unref(varp->vval.v_list);
		break;
	    case VAR_DICT:
		dict_unref(varp->vval.v_dict);
		break;
	    case VAR_JOB:
#ifdef FEAT_JOB_CHANNEL
		job_unref(varp->vval.v_job);
		break;
#endif
	    case VAR_CHANNEL:
#ifdef FEAT_JOB_CHANNEL
		channel_unref(varp->vval.v_channel);
		break;
#endif
	    case VAR_NUMBER:
	    case VAR_FLOAT:
	    case VAR_ANY:
	    case VAR_UNKNOWN:
	    case VAR_VOID:
	    case VAR_BOOL:
	    case VAR_SPECIAL:
	    case VAR_INSTR:
		break;
	}
	vim_free(varp);
    }
}