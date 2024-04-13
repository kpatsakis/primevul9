compile_arguments(char_u **arg, cctx_T *cctx, int *argcount, int is_searchpair)
{
    char_u  *p = *arg;
    char_u  *whitep = *arg;
    int	    must_end = FALSE;
    int	    instr_count;

    for (;;)
    {
	if (may_get_next_line(whitep, &p, cctx) == FAIL)
	    goto failret;
	if (*p == ')')
	{
	    *arg = p + 1;
	    return OK;
	}
	if (must_end)
	{
	    semsg(_(e_missing_comma_before_argument_str), p);
	    return FAIL;
	}

	instr_count = cctx->ctx_instr.ga_len;
	if (compile_expr0(&p, cctx) == FAIL)
	    return FAIL;
	++*argcount;

	if (is_searchpair && *argcount == 5
		&& cctx->ctx_instr.ga_len == instr_count + 1)
	{
	    isn_T *isn = ((isn_T *)cctx->ctx_instr.ga_data) + instr_count;

	    // {skip} argument of searchpair() can be compiled if not empty
	    if (isn->isn_type == ISN_PUSHS && *isn->isn_arg.string != NUL)
		compile_string(isn, cctx);
	}

	if (*p != ',' && *skipwhite(p) == ',')
	{
	    semsg(_(e_no_white_space_allowed_before_str_str), ",", p);
	    p = skipwhite(p);
	}
	if (*p == ',')
	{
	    ++p;
	    if (*p != NUL && !VIM_ISWHITE(*p))
		semsg(_(e_white_space_required_after_str_str), ",", p - 1);
	}
	else
	    must_end = TRUE;
	whitep = p;
	p = skipwhite(p);
    }
failret:
    emsg(_(e_missing_closing_paren));
    return FAIL;
}