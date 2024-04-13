typval_compare(
    typval_T	*tv1,	// first operand
    typval_T	*tv2,	// second operand
    exprtype_T	type,   // operator
    int		ic)     // ignore case
{
    varnumber_T	n1, n2;
    int		res = 0;
    int		type_is = type == EXPR_IS || type == EXPR_ISNOT;

    if (type_is && tv1->v_type != tv2->v_type)
    {
	// For "is" a different type always means FALSE, for "notis"
	// it means TRUE.
	n1 = (type == EXPR_ISNOT);
    }
    else if (((tv1->v_type == VAR_SPECIAL && tv1->vval.v_number == VVAL_NULL)
		|| (tv2->v_type == VAR_SPECIAL
					   && tv2->vval.v_number == VVAL_NULL))
	    && tv1->v_type != tv2->v_type
	    && (type == EXPR_EQUAL || type == EXPR_NEQUAL))
    {
	n1 = typval_compare_null(tv1, tv2);
	if (n1 == MAYBE)
	{
	    clear_tv(tv1);
	    return FAIL;
	}
	if (type == EXPR_NEQUAL)
	    n1 = !n1;
    }
    else if (tv1->v_type == VAR_BLOB || tv2->v_type == VAR_BLOB)
    {
	if (typval_compare_blob(tv1, tv2, type, &res) == FAIL)
	{
	    clear_tv(tv1);
	    return FAIL;
	}
	n1 = res;
    }
    else if (tv1->v_type == VAR_LIST || tv2->v_type == VAR_LIST)
    {
	if (typval_compare_list(tv1, tv2, type, ic, &res) == FAIL)
	{
	    clear_tv(tv1);
	    return FAIL;
	}
	n1 = res;
    }
    else if (tv1->v_type == VAR_DICT || tv2->v_type == VAR_DICT)
    {
	if (typval_compare_dict(tv1, tv2, type, ic, &res) == FAIL)
	{
	    clear_tv(tv1);
	    return FAIL;
	}
	n1 = res;
    }
    else if (tv1->v_type == VAR_FUNC || tv2->v_type == VAR_FUNC
	|| tv1->v_type == VAR_PARTIAL || tv2->v_type == VAR_PARTIAL)
    {
	if (typval_compare_func(tv1, tv2, type, ic, &res) == FAIL)
	{
	    clear_tv(tv1);
	    return FAIL;
	}
	n1 = res;
    }

#ifdef FEAT_FLOAT
    // If one of the two variables is a float, compare as a float.
    // When using "=~" or "!~", always compare as string.
    else if ((tv1->v_type == VAR_FLOAT || tv2->v_type == VAR_FLOAT)
	    && type != EXPR_MATCH && type != EXPR_NOMATCH)
    {
	float_T f1, f2;
	int	error = FALSE;

	f1 = tv_get_float_chk(tv1, &error);
	if (!error)
	    f2 = tv_get_float_chk(tv2, &error);
	if (error)
	{
	    clear_tv(tv1);
	    return FAIL;
	}
	n1 = FALSE;
	switch (type)
	{
	    case EXPR_IS:
	    case EXPR_EQUAL:    n1 = (f1 == f2); break;
	    case EXPR_ISNOT:
	    case EXPR_NEQUAL:   n1 = (f1 != f2); break;
	    case EXPR_GREATER:  n1 = (f1 > f2); break;
	    case EXPR_GEQUAL:   n1 = (f1 >= f2); break;
	    case EXPR_SMALLER:  n1 = (f1 < f2); break;
	    case EXPR_SEQUAL:   n1 = (f1 <= f2); break;
	    case EXPR_UNKNOWN:
	    case EXPR_MATCH:
	    default:  break;  // avoid gcc warning
	}
    }
#endif

    // If one of the two variables is a number, compare as a number.
    // When using "=~" or "!~", always compare as string.
    else if ((tv1->v_type == VAR_NUMBER || tv2->v_type == VAR_NUMBER)
	    && type != EXPR_MATCH && type != EXPR_NOMATCH)
    {
	int error = FALSE;

	n1 = tv_get_number_chk(tv1, &error);
	if (!error)
	    n2 = tv_get_number_chk(tv2, &error);
	if (error)
	{
	    clear_tv(tv1);
	    return FAIL;
	}
	switch (type)
	{
	    case EXPR_IS:
	    case EXPR_EQUAL:    n1 = (n1 == n2); break;
	    case EXPR_ISNOT:
	    case EXPR_NEQUAL:   n1 = (n1 != n2); break;
	    case EXPR_GREATER:  n1 = (n1 > n2); break;
	    case EXPR_GEQUAL:   n1 = (n1 >= n2); break;
	    case EXPR_SMALLER:  n1 = (n1 < n2); break;
	    case EXPR_SEQUAL:   n1 = (n1 <= n2); break;
	    case EXPR_UNKNOWN:
	    case EXPR_MATCH:
	    default:  break;  // avoid gcc warning
	}
    }
    else if (in_vim9script() && (tv1->v_type == VAR_BOOL
				    || tv2->v_type == VAR_BOOL
				    || (tv1->v_type == VAR_SPECIAL
					      && tv2->v_type == VAR_SPECIAL)))
    {
	if (tv1->v_type != tv2->v_type)
	{
	    semsg(_(e_cannot_compare_str_with_str),
		       vartype_name(tv1->v_type), vartype_name(tv2->v_type));
	    clear_tv(tv1);
	    return FAIL;
	}
	n1 = tv1->vval.v_number;
	n2 = tv2->vval.v_number;
	switch (type)
	{
	    case EXPR_IS:
	    case EXPR_EQUAL:    n1 = (n1 == n2); break;
	    case EXPR_ISNOT:
	    case EXPR_NEQUAL:   n1 = (n1 != n2); break;
	    default:
		semsg(_(e_invalid_operation_for_str),
						   vartype_name(tv1->v_type));
		clear_tv(tv1);
		return FAIL;
	}
    }
#ifdef FEAT_JOB_CHANNEL
    else if (tv1->v_type == tv2->v_type
	    && (tv1->v_type == VAR_CHANNEL || tv1->v_type == VAR_JOB)
	    && (type == EXPR_NEQUAL || type == EXPR_EQUAL))
    {
	if (tv1->v_type == VAR_CHANNEL)
	    n1 = tv1->vval.v_channel == tv2->vval.v_channel;
	else
	    n1 = tv1->vval.v_job == tv2->vval.v_job;
	if (type == EXPR_NEQUAL)
	    n1 = !n1;
    }
#endif
    else
    {
	if (typval_compare_string(tv1, tv2, type, ic, &res) == FAIL)
	{
	    clear_tv(tv1);
	    return FAIL;
	}
	n1 = res;
    }
    clear_tv(tv1);
    if (in_vim9script())
    {
	tv1->v_type = VAR_BOOL;
	tv1->vval.v_number = n1 ? VVAL_TRUE : VVAL_FALSE;
    }
    else
    {
	tv1->v_type = VAR_NUMBER;
	tv1->vval.v_number = n1;
    }

    return OK;
}