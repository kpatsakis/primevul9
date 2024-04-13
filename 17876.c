typval_compare_null(typval_T *tv1, typval_T *tv2)
{
    if ((tv1->v_type == VAR_SPECIAL && tv1->vval.v_number == VVAL_NULL)
	    || (tv2->v_type == VAR_SPECIAL && tv2->vval.v_number == VVAL_NULL))
    {
	typval_T	*tv = tv1->v_type == VAR_SPECIAL ? tv2 : tv1;

	switch (tv->v_type)
	{
	    case VAR_BLOB: return tv->vval.v_blob == NULL;
#ifdef FEAT_JOB_CHANNEL
	    case VAR_CHANNEL: return tv->vval.v_channel == NULL;
#endif
	    case VAR_DICT: return tv->vval.v_dict == NULL;
	    case VAR_FUNC: return tv->vval.v_string == NULL;
#ifdef FEAT_JOB_CHANNEL
	    case VAR_JOB: return tv->vval.v_job == NULL;
#endif
	    case VAR_LIST: return tv->vval.v_list == NULL;
	    case VAR_PARTIAL: return tv->vval.v_partial == NULL;
	    case VAR_STRING: return tv->vval.v_string == NULL;

	    case VAR_NUMBER: if (!in_vim9script())
				 return tv->vval.v_number == 0;
			     break;
#ifdef FEAT_FLOAT
	    case VAR_FLOAT: if (!in_vim9script())
				 return tv->vval.v_float == 0.0;
			     break;
#endif
	    default: break;
	}
    }
    // although comparing null with number, float or bool is not very useful
    // we won't give an error
    return FALSE;
}