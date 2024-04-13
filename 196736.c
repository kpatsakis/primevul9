compile_member(int is_slice, int *keeping_dict, cctx_T *cctx)
{
    type2_T	*typep;
    garray_T	*stack = &cctx->ctx_type_stack;
    vartype_T	vartype;
    type_T	*idxtype;

    // We can index a list, dict and blob.  If we don't know the type
    // we can use the index value type.  If we still don't know use an "ANY"
    // instruction.
    // TODO: what about the decl type?
    typep = (((type2_T *)stack->ga_data) + stack->ga_len - (is_slice ? 3 : 2));
    vartype = typep->type_curr->tt_type;
    idxtype = (((type2_T *)stack->ga_data) + stack->ga_len - 1)->type_curr;
    // If the index is a string, the variable must be a Dict.
    if ((typep->type_curr == &t_any || typep->type_curr == &t_unknown)
						       && idxtype == &t_string)
	vartype = VAR_DICT;
    if (vartype == VAR_STRING || vartype == VAR_LIST || vartype == VAR_BLOB)
    {
	if (need_type(idxtype, &t_number, -1, 0, cctx, FALSE, FALSE) == FAIL)
	    return FAIL;
	if (is_slice)
	{
	    idxtype = get_type_on_stack(cctx, 1);
	    if (need_type(idxtype, &t_number, -2, 0, cctx,
							 FALSE, FALSE) == FAIL)
		return FAIL;
	}
    }

    if (vartype == VAR_DICT)
    {
	if (is_slice)
	{
	    emsg(_(e_cannot_slice_dictionary));
	    return FAIL;
	}
	if (typep->type_curr->tt_type == VAR_DICT)
	{
	    typep->type_curr = typep->type_curr->tt_member;
	    if (typep->type_curr == &t_unknown)
		// empty dict was used
		typep->type_curr = &t_any;
	    if (typep->type_decl->tt_type == VAR_DICT)
	    {
		typep->type_decl = typep->type_decl->tt_member;
		if (typep->type_decl == &t_unknown)
		    // empty dict was used
		    typep->type_decl = &t_any;
	    }
	    else
		typep->type_decl = typep->type_curr;
	}
	else
	{
	    if (need_type(typep->type_curr, &t_dict_any, -2, 0, cctx,
							 FALSE, FALSE) == FAIL)
		return FAIL;
	    typep->type_curr = &t_any;
	    typep->type_decl = &t_any;
	}
	if (may_generate_2STRING(-1, FALSE, cctx) == FAIL)
	    return FAIL;
	if (generate_instr_drop(cctx, ISN_MEMBER, 1) == FAIL)
	    return FAIL;
	if (keeping_dict != NULL)
	    *keeping_dict = TRUE;
    }
    else if (vartype == VAR_STRING)
    {
	typep->type_curr = &t_string;
	typep->type_decl = &t_string;
	if ((is_slice
		? generate_instr_drop(cctx, ISN_STRSLICE, 2)
		: generate_instr_drop(cctx, ISN_STRINDEX, 1)) == FAIL)
	    return FAIL;
    }
    else if (vartype == VAR_BLOB)
    {
	if (is_slice)
	{
	    typep->type_curr = &t_blob;
	    typep->type_decl = &t_blob;
	    if (generate_instr_drop(cctx, ISN_BLOBSLICE, 2) == FAIL)
		return FAIL;
	}
	else
	{
	    typep->type_curr = &t_number;
	    typep->type_decl = &t_number;
	    if (generate_instr_drop(cctx, ISN_BLOBINDEX, 1) == FAIL)
		return FAIL;
	}
    }
    else if (vartype == VAR_LIST || typep->type_curr == &t_any
					     || typep->type_curr == &t_unknown)
    {
	if (is_slice)
	{
	    if (generate_instr_drop(cctx,
		     vartype == VAR_LIST ?  ISN_LISTSLICE : ISN_ANYSLICE,
							    2) == FAIL)
		return FAIL;
	}
	else
	{
	    if (typep->type_curr->tt_type == VAR_LIST)
	    {
		typep->type_curr = typep->type_curr->tt_member;
		if (typep->type_curr == &t_unknown)
		    // empty list was used
		    typep->type_curr = &t_any;
		if (typep->type_decl->tt_type == VAR_LIST)
		{
		    typep->type_decl = typep->type_decl->tt_member;
		    if (typep->type_decl == &t_unknown)
			// empty list was used
			typep->type_decl = &t_any;
		}
		else
			typep->type_decl = typep->type_curr;
	    }
	    if (generate_instr_drop(cctx,
			vartype == VAR_LIST ?  ISN_LISTINDEX : ISN_ANYINDEX, 1)
								       == FAIL)
		return FAIL;
	}
    }
    else
    {
	switch (vartype)
	{
	    case VAR_FUNC:
	    case VAR_PARTIAL:
		emsg(_(e_cannot_index_a_funcref));
		break;
	    case VAR_BOOL:
	    case VAR_SPECIAL:
	    case VAR_JOB:
	    case VAR_CHANNEL:
	    case VAR_INSTR:
	    case VAR_UNKNOWN:
	    case VAR_ANY:
	    case VAR_VOID:
		emsg(_(e_cannot_index_special_variable));
		break;
	    default:
		emsg(_(e_string_list_dict_or_blob_required));
	}
	return FAIL;
    }
    return OK;
}