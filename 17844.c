typval_compare_blob(
	typval_T    *tv1,
	typval_T    *tv2,
	exprtype_T  type,
	int	    *res)
{
    int	    val = 0;

    if (type == EXPR_IS || type == EXPR_ISNOT)
    {
	val = (tv1->v_type == tv2->v_type
			&& tv1->vval.v_blob == tv2->vval.v_blob);
	if (type == EXPR_ISNOT)
	    val = !val;
    }
    else if (tv1->v_type != tv2->v_type
	    || (type != EXPR_EQUAL && type != EXPR_NEQUAL))
    {
	if (tv1->v_type != tv2->v_type)
	    emsg(_(e_can_only_compare_blob_with_blob));
	else
	    emsg(_(e_invalid_operation_for_blob));
	return FAIL;
    }
    else
    {
	val = blob_equal(tv1->vval.v_blob, tv2->vval.v_blob);
	if (type == EXPR_NEQUAL)
	    val = !val;
    }
    *res = val;
    return OK;
}