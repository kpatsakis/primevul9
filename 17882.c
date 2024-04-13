typval_compare_func(
	typval_T    *tv1,
	typval_T    *tv2,
	exprtype_T  type,
	int	    ic,
	int	    *res)
{
    int	    val = 0;

    if (type != EXPR_EQUAL && type != EXPR_NEQUAL
	    && type != EXPR_IS && type != EXPR_ISNOT)
    {
	emsg(_(e_invalid_operation_for_funcrefs));
	return FAIL;
    }
    if ((tv1->v_type == VAR_PARTIAL && tv1->vval.v_partial == NULL)
	    || (tv2->v_type == VAR_PARTIAL && tv2->vval.v_partial == NULL))
	// When both partials are NULL, then they are equal.
	// Otherwise they are not equal.
	val = (tv1->vval.v_partial == tv2->vval.v_partial);
    else if (type == EXPR_IS || type == EXPR_ISNOT)
    {
	if (tv1->v_type == VAR_FUNC && tv2->v_type == VAR_FUNC)
	    // strings are considered the same if their value is
	    // the same
	    val = tv_equal(tv1, tv2, ic, FALSE);
	else if (tv1->v_type == VAR_PARTIAL && tv2->v_type == VAR_PARTIAL)
	    val = (tv1->vval.v_partial == tv2->vval.v_partial);
	else
	    val = FALSE;
    }
    else
	val = tv_equal(tv1, tv2, ic, FALSE);
    if (type == EXPR_NEQUAL || type == EXPR_ISNOT)
	val = !val;
    *res = val;
    return OK;
}