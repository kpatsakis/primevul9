compile_and_or(
	char_u **arg,
	cctx_T	*cctx,
	char	*op,
	ppconst_T *ppconst,
	int	ppconst_used UNUSED)
{
    char_u	*next;
    char_u	*p = may_peek_next_line(cctx, *arg, &next);
    int		opchar = *op;

    if (p[0] == opchar && p[1] == opchar)
    {
	garray_T	*instr = &cctx->ctx_instr;
	garray_T	end_ga;
	int		save_skip = cctx->ctx_skip;

	/*
	 * Repeat until there is no following "||" or "&&"
	 */
	ga_init2(&end_ga, sizeof(int), 10);
	while (p[0] == opchar && p[1] == opchar)
	{
	    long	start_lnum = SOURCING_LNUM;
	    long	save_sourcing_lnum;
	    int		start_ctx_lnum = cctx->ctx_lnum;
	    int		save_lnum;
	    int		const_used;
	    int		status;
	    jumpwhen_T	jump_when = opchar == '|'
				      ? JUMP_IF_COND_TRUE : JUMP_IF_COND_FALSE;

	    if (next != NULL)
	    {
		*arg = next_line_from_context(cctx, TRUE);
		p = skipwhite(*arg);
	    }

	    if (!IS_WHITE_OR_NUL(**arg) || !IS_WHITE_OR_NUL(p[2]))
	    {
		semsg(_(e_white_space_required_before_and_after_str_at_str),
									op, p);
		ga_clear(&end_ga);
		return FAIL;
	    }

	    save_sourcing_lnum = SOURCING_LNUM;
	    SOURCING_LNUM = start_lnum;
	    save_lnum = cctx->ctx_lnum;
	    cctx->ctx_lnum = start_ctx_lnum;

	    status = check_ppconst_bool(ppconst);
	    if (status != FAIL)
	    {
		// Use the last ppconst if possible.
		if (ppconst->pp_used > 0)
		{
		    typval_T	*tv = &ppconst->pp_tv[ppconst->pp_used - 1];
		    int		is_true = tv2bool(tv);

		    if ((is_true && opchar == '|')
						|| (!is_true && opchar == '&'))
		    {
			// For "false && expr" and "true || expr" the "expr"
			// does not need to be evaluated.
			cctx->ctx_skip = SKIP_YES;
			clear_tv(tv);
			tv->v_type = VAR_BOOL;
			tv->vval.v_number = is_true ? VVAL_TRUE : VVAL_FALSE;
		    }
		    else
		    {
			// For "true && expr" and "false || expr" only "expr"
			// needs to be evaluated.
			--ppconst->pp_used;
			jump_when = JUMP_NEVER;
		    }
		}
		else
		{
		    // Every part must evaluate to a bool.
		    status = bool_on_stack(cctx);
		}
	    }
	    if (status != FAIL)
		status = ga_grow(&end_ga, 1);
	    cctx->ctx_lnum = save_lnum;
	    if (status == FAIL)
	    {
		ga_clear(&end_ga);
		return FAIL;
	    }

	    if (jump_when != JUMP_NEVER)
	    {
		if (cctx->ctx_skip != SKIP_YES)
		{
		    *(((int *)end_ga.ga_data) + end_ga.ga_len) = instr->ga_len;
		    ++end_ga.ga_len;
		}
		generate_JUMP(cctx, jump_when, 0);
	    }

	    // eval the next expression
	    SOURCING_LNUM = save_sourcing_lnum;
	    if (may_get_next_line_error(p + 2, arg, cctx) == FAIL)
	    {
		ga_clear(&end_ga);
		return FAIL;
	    }

	    const_used = ppconst->pp_used;
	    if ((opchar == '|' ? compile_expr3(arg, cctx, ppconst)
				  : compile_expr4(arg, cctx, ppconst)) == FAIL)
	    {
		ga_clear(&end_ga);
		return FAIL;
	    }

	    // "0 || 1" results in true, "1 && 0" results in false.
	    if (ppconst->pp_used == const_used + 1)
	    {
		typval_T	*tv = &ppconst->pp_tv[ppconst->pp_used - 1];

		if (tv->v_type == VAR_NUMBER
			 && (tv->vval.v_number == 1 || tv->vval.v_number == 0))
		{
		    tv->vval.v_number = tv->vval.v_number == 1
						      ? VVAL_TRUE : VVAL_FALSE;
		    tv->v_type = VAR_BOOL;
		}
	    }

	    p = may_peek_next_line(cctx, *arg, &next);
	}

	if (check_ppconst_bool(ppconst) == FAIL)
	{
	    ga_clear(&end_ga);
	    return FAIL;
	}

	if (cctx->ctx_skip != SKIP_YES && ppconst->pp_used == 0)
	    // Every part must evaluate to a bool.
	    if (bool_on_stack(cctx) == FAIL)
	    {
		ga_clear(&end_ga);
		return FAIL;
	    }

	if (end_ga.ga_len > 0)
	{
	    // Fill in the end label in all jumps.
	    generate_ppconst(cctx, ppconst);
	    while (end_ga.ga_len > 0)
	    {
		isn_T	*isn;

		--end_ga.ga_len;
		isn = ((isn_T *)instr->ga_data)
				  + *(((int *)end_ga.ga_data) + end_ga.ga_len);
		isn->isn_arg.jump.jump_where = instr->ga_len;
	    }
	}
	ga_clear(&end_ga);

	cctx->ctx_skip = save_skip;
    }

    return OK;
}