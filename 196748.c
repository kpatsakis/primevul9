compile_list(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    char_u	*p = skipwhite(*arg + 1);
    char_u	*whitep = *arg + 1;
    int		count = 0;
    int		is_const;
    int		is_all_const = TRUE;	// reset when non-const encountered

    for (;;)
    {
	if (may_get_next_line(whitep, &p, cctx) == FAIL)
	{
	    semsg(_(e_missing_end_of_list_rsb_str), *arg);
	    return FAIL;
	}
	if (*p == ',')
	{
	    semsg(_(e_no_white_space_allowed_before_str_str), ",", p);
	    return FAIL;
	}
	if (*p == ']')
	{
	    ++p;
	    break;
	}
	if (compile_expr0_ext(&p, cctx, &is_const) == FAIL)
	    return FAIL;
	if (!is_const)
	    is_all_const = FALSE;
	++count;
	if (*p == ',')
	{
	    ++p;
	    if (*p != ']' && !IS_WHITE_OR_NUL(*p))
	    {
		semsg(_(e_white_space_required_after_str_str), ",", p - 1);
		return FAIL;
	    }
	}
	whitep = p;
	p = skipwhite(p);
    }
    *arg = p;

    ppconst->pp_is_const = is_all_const;
    return generate_NEWLIST(cctx, count);
}