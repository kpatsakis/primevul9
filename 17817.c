typval_compare_list(
	typval_T    *tv1,
	typval_T    *tv2,
	exprtype_T  type,
	int	    ic,
	int	    *res)
{
    int	    val = 0;

    if (type == EXPR_IS || type == EXPR_ISNOT)
    {
	val = (tv1->v_type == tv2->v_type
				      && tv1->vval.v_list == tv2->vval.v_list);
	if (type == EXPR_ISNOT)
	    val = !val;
    }
    else if (tv1->v_type != tv2->v_type
	    || (type != EXPR_EQUAL && type != EXPR_NEQUAL))
    {
	if (tv1->v_type != tv2->v_type)
	    emsg(_(e_can_only_compare_list_with_list));
	else
	    emsg(_(e_invalid_operation_for_list));
	return FAIL;
    }
    else
    {
	val = list_equal(tv1->vval.v_list, tv2->vval.v_list,
							ic, FALSE);
	if (type == EXPR_NEQUAL)
	    val = !val;
    }
    *res = val;
    return OK;
}