typval_compare_dict(
	typval_T    *tv1,
	typval_T    *tv2,
	exprtype_T  type,
	int	    ic,
	int	    *res)
{
    int	    val;

    if (type == EXPR_IS || type == EXPR_ISNOT)
    {
	val = (tv1->v_type == tv2->v_type
			&& tv1->vval.v_dict == tv2->vval.v_dict);
	if (type == EXPR_ISNOT)
	    val = !val;
    }
    else if (tv1->v_type != tv2->v_type
		|| (type != EXPR_EQUAL && type != EXPR_NEQUAL))
    {
	if (tv1->v_type != tv2->v_type)
	    emsg(_(e_can_only_compare_dictionary_with_dictionary));
	else
	    emsg(_(e_invalid_operation_for_dictionary));
	return FAIL;
    }
    else
    {
	val = dict_equal(tv1->vval.v_dict, tv2->vval.v_dict, ic, FALSE);
	if (type == EXPR_NEQUAL)
	    val = !val;
    }
    *res = val;
    return OK;
}