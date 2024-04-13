check_can_index(typval_T *rettv, int evaluate, int verbose)
{
    switch (rettv->v_type)
    {
	case VAR_FUNC:
	case VAR_PARTIAL:
	    if (verbose)
		emsg(_("E695: Cannot index a Funcref"));
	    return FAIL;
	case VAR_FLOAT:
#ifdef FEAT_FLOAT
	    if (verbose)
		emsg(_(e_float_as_string));
	    return FAIL;
#endif
	case VAR_BOOL:
	case VAR_SPECIAL:
	case VAR_JOB:
	case VAR_CHANNEL:
	case VAR_INSTR:
	    if (verbose)
		emsg(_(e_cannot_index_special_variable));
	    return FAIL;
	case VAR_UNKNOWN:
	case VAR_ANY:
	case VAR_VOID:
	    if (evaluate)
	    {
		emsg(_(e_cannot_index_special_variable));
		return FAIL;
	    }
	    // FALLTHROUGH

	case VAR_STRING:
	case VAR_LIST:
	case VAR_DICT:
	case VAR_BLOB:
	    break;
	case VAR_NUMBER:
	    if (in_vim9script())
		emsg(_(e_cannot_index_number));
	    break;
    }
    return OK;
}