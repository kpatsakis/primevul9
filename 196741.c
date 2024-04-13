compile_expr1(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    char_u	*p;
    int		ppconst_used = ppconst->pp_used;
    char_u	*next;

    // Ignore all kinds of errors when not producing code.
    if (cctx->ctx_skip == SKIP_YES)
    {
	skip_expr_cctx(arg, cctx);
	return OK;
    }

    // Evaluate the first expression.
    if (compile_expr2(arg, cctx, ppconst) == FAIL)
	return FAIL;

    p = may_peek_next_line(cctx, *arg, &next);
    if (*p == '?')
    {
	int		op_falsy = p[1] == '?';
	garray_T	*instr = &cctx->ctx_instr;
	garray_T	*stack = &cctx->ctx_type_stack;
	int		alt_idx = instr->ga_len;
	int		end_idx = 0;
	isn_T		*isn;
	type_T		*type1 = NULL;
	int		has_const_expr = FALSE;
	int		const_value = FALSE;
	int		save_skip = cctx->ctx_skip;

	if (next != NULL)
	{
	    *arg = next_line_from_context(cctx, TRUE);
	    p = skipwhite(*arg);
	}

	if (!IS_WHITE_OR_NUL(**arg) || !IS_WHITE_OR_NUL(p[1 + op_falsy]))
	{
	    semsg(_(e_white_space_required_before_and_after_str_at_str),
						     op_falsy ? "??" : "?", p);
	    return FAIL;
	}

	if (ppconst->pp_used == ppconst_used + 1)
	{
	    // the condition is a constant, we know whether the ? or the :
	    // expression is to be evaluated.
	    has_const_expr = TRUE;
	    if (op_falsy)
		const_value = tv2bool(&ppconst->pp_tv[ppconst_used]);
	    else
	    {
		int error = FALSE;

		const_value = tv_get_bool_chk(&ppconst->pp_tv[ppconst_used],
								       &error);
		if (error)
		    return FAIL;
	    }
	    cctx->ctx_skip = save_skip == SKIP_YES ||
		 (op_falsy ? const_value : !const_value) ? SKIP_YES : SKIP_NOT;

	    if (op_falsy && cctx->ctx_skip == SKIP_YES)
		// "left ?? right" and "left" is truthy: produce "left"
		generate_ppconst(cctx, ppconst);
	    else
	    {
		clear_tv(&ppconst->pp_tv[ppconst_used]);
		--ppconst->pp_used;
	    }
	}
	else
	{
	    generate_ppconst(cctx, ppconst);
	    if (op_falsy)
		end_idx = instr->ga_len;
	    generate_JUMP(cctx, op_falsy
				   ? JUMP_AND_KEEP_IF_TRUE : JUMP_IF_FALSE, 0);
	    if (op_falsy)
		type1 = get_type_on_stack(cctx, -1);
	}

	// evaluate the second expression; any type is accepted
	if (may_get_next_line_error(p + 1 + op_falsy, arg, cctx) == FAIL)
	    return FAIL;
	if (compile_expr1(arg, cctx, ppconst) == FAIL)
	    return FAIL;

	if (!has_const_expr)
	{
	    generate_ppconst(cctx, ppconst);

	    if (!op_falsy)
	    {
		// remember the type and drop it
		type1 = get_type_on_stack(cctx, 0);
		--stack->ga_len;

		end_idx = instr->ga_len;
		generate_JUMP(cctx, JUMP_ALWAYS, 0);

		// jump here from JUMP_IF_FALSE
		isn = ((isn_T *)instr->ga_data) + alt_idx;
		isn->isn_arg.jump.jump_where = instr->ga_len;
	    }
	}

	if (!op_falsy)
	{
	    // Check for the ":".
	    p = may_peek_next_line(cctx, *arg, &next);
	    if (*p != ':')
	    {
		emsg(_(e_missing_colon_after_questionmark));
		return FAIL;
	    }
	    if (next != NULL)
	    {
		*arg = next_line_from_context(cctx, TRUE);
		p = skipwhite(*arg);
	    }

	    if (!IS_WHITE_OR_NUL(**arg) || !IS_WHITE_OR_NUL(p[1]))
	    {
		semsg(_(e_white_space_required_before_and_after_str_at_str),
								       ":", p);
		return FAIL;
	    }

	    // evaluate the third expression
	    if (has_const_expr)
		cctx->ctx_skip = save_skip == SKIP_YES || const_value
							 ? SKIP_YES : SKIP_NOT;
	    if (may_get_next_line_error(p + 1, arg, cctx) == FAIL)
		return FAIL;
	    if (compile_expr1(arg, cctx, ppconst) == FAIL)
		return FAIL;
	}

	if (!has_const_expr)
	{
	    type_T	**typep;

	    generate_ppconst(cctx, ppconst);
	    ppconst->pp_is_const = FALSE;

	    // If the types differ, the result has a more generic type.
	    typep = &((((type2_T *)stack->ga_data)
					      + stack->ga_len - 1)->type_curr);
	    common_type(type1, *typep, typep, cctx->ctx_type_list);

	    // jump here from JUMP_ALWAYS or JUMP_AND_KEEP_IF_TRUE
	    isn = ((isn_T *)instr->ga_data) + end_idx;
	    isn->isn_arg.jump.jump_where = instr->ga_len;
	}

	cctx->ctx_skip = save_skip;
    }
    return OK;
}