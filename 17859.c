func_equal(
    typval_T *tv1,
    typval_T *tv2,
    int	     ic)	    // ignore case
{
    char_u	*s1, *s2;
    dict_T	*d1, *d2;
    int		a1, a2;
    int		i;

    // empty and NULL function name considered the same
    s1 = tv1->v_type == VAR_FUNC ? tv1->vval.v_string
					   : partial_name(tv1->vval.v_partial);
    if (s1 != NULL && *s1 == NUL)
	s1 = NULL;
    s2 = tv2->v_type == VAR_FUNC ? tv2->vval.v_string
					   : partial_name(tv2->vval.v_partial);
    if (s2 != NULL && *s2 == NUL)
	s2 = NULL;
    if (s1 == NULL || s2 == NULL)
    {
	if (s1 != s2)
	    return FALSE;
    }
    else if (STRCMP(s1, s2) != 0)
	return FALSE;

    // empty dict and NULL dict is different
    d1 = tv1->v_type == VAR_FUNC ? NULL : tv1->vval.v_partial->pt_dict;
    d2 = tv2->v_type == VAR_FUNC ? NULL : tv2->vval.v_partial->pt_dict;
    if (d1 == NULL || d2 == NULL)
    {
	if (d1 != d2)
	    return FALSE;
    }
    else if (!dict_equal(d1, d2, ic, TRUE))
	return FALSE;

    // empty list and no list considered the same
    a1 = tv1->v_type == VAR_FUNC ? 0 : tv1->vval.v_partial->pt_argc;
    a2 = tv2->v_type == VAR_FUNC ? 0 : tv2->vval.v_partial->pt_argc;
    if (a1 != a2)
	return FALSE;
    for (i = 0; i < a1; ++i)
	if (!tv_equal(tv1->vval.v_partial->pt_argv + i,
		      tv2->vval.v_partial->pt_argv + i, ic, TRUE))
	    return FALSE;

    return TRUE;
}