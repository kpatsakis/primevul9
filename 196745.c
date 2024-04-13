compile_subscript(
	char_u **arg,
	cctx_T *cctx,
	char_u *start_leader,
	char_u **end_leader,
	ppconst_T *ppconst)
{
    char_u	*name_start = *end_leader;
    int		keeping_dict = FALSE;

    for (;;)
    {
	char_u *p = skipwhite(*arg);

	if (*p == NUL || (VIM_ISWHITE(**arg) && vim9_comment_start(p)))
	{
	    char_u *next = peek_next_line_from_context(cctx);

	    // If a following line starts with "->{" or "->X" advance to that
	    // line, so that a line break before "->" is allowed.
	    // Also if a following line starts with ".x".
	    if (next != NULL &&
		    ((next[0] == '-' && next[1] == '>'
				 && (next[2] == '{'
				       || ASCII_ISALPHA(*skipwhite(next + 2))))
		    || (next[0] == '.' && eval_isdictc(next[1]))))
	    {
		next = next_line_from_context(cctx, TRUE);
		if (next == NULL)
		    return FAIL;
		*arg = next;
		p = skipwhite(*arg);
	    }
	}

	// Do not skip over white space to find the "(", "execute 'x' (expr)"
	// is not a function call.
	if (**arg == '(')
	{
	    type_T	*type;
	    int		argcount = 0;

	    if (generate_ppconst(cctx, ppconst) == FAIL)
		return FAIL;
	    ppconst->pp_is_const = FALSE;

	    // funcref(arg)
	    type = get_type_on_stack(cctx, 0);

	    *arg = skipwhite(p + 1);
	    if (compile_arguments(arg, cctx, &argcount, FALSE) == FAIL)
		return FAIL;
	    if (generate_PCALL(cctx, argcount, name_start, type, TRUE) == FAIL)
		return FAIL;
	    if (keeping_dict)
	    {
		keeping_dict = FALSE;
		if (generate_instr(cctx, ISN_CLEARDICT) == NULL)
		    return FAIL;
	    }
	}
	else if (*p == '-' && p[1] == '>')
	{
	    char_u *pstart = p;

	    if (generate_ppconst(cctx, ppconst) == FAIL)
		return FAIL;
	    ppconst->pp_is_const = FALSE;

	    // something->method()
	    // Apply the '!', '-' and '+' first:
	    //   -1.0->func() works like (-1.0)->func()
	    if (compile_leader(cctx, TRUE, start_leader, end_leader) == FAIL)
		return FAIL;

	    p += 2;
	    *arg = skipwhite(p);
	    // No line break supported right after "->".
	    if (**arg == '(')
	    {
		int	    argcount = 1;
		garray_T    *stack = &cctx->ctx_type_stack;
		int	    type_idx_start = stack->ga_len;
		type_T	    *type;
		int	    expr_isn_start = cctx->ctx_instr.ga_len;
		int	    expr_isn_end;
		int	    arg_isn_count;

		// Funcref call:  list->(Refs[2])(arg)
		// or lambda:	  list->((arg) => expr)(arg)
		//
		// Fist compile the function expression.
		if (compile_parenthesis(arg, cctx, ppconst) == FAIL)
		    return FAIL;

		// Remember the next instruction index, where the instructions
		// for arguments are being written.
		expr_isn_end = cctx->ctx_instr.ga_len;

		// Compile the arguments.
		if (**arg != '(')
		{
		    if (*skipwhite(*arg) == '(')
			emsg(_(e_no_white_space_allowed_before_parenthesis));
		    else
			semsg(_(e_missing_parenthesis_str), *arg);
		    return FAIL;
		}
		*arg = skipwhite(*arg + 1);
		if (compile_arguments(arg, cctx, &argcount, FALSE) == FAIL)
		    return FAIL;

		// Move the instructions for the arguments to before the
		// instructions of the expression and move the type of the
		// expression after the argument types.  This is what ISN_PCALL
		// expects.
		arg_isn_count = cctx->ctx_instr.ga_len - expr_isn_end;
		if (arg_isn_count > 0)
		{
		    int	    expr_isn_count = expr_isn_end - expr_isn_start;
		    isn_T   *isn = ALLOC_MULT(isn_T, expr_isn_count);
		    type_T  *decl_type;
		    type2_T  *typep;

		    if (isn == NULL)
			return FAIL;
		    mch_memmove(isn, ((isn_T *)cctx->ctx_instr.ga_data)
							      + expr_isn_start,
					       sizeof(isn_T) * expr_isn_count);
		    mch_memmove(((isn_T *)cctx->ctx_instr.ga_data)
							      + expr_isn_start,
			     ((isn_T *)cctx->ctx_instr.ga_data) + expr_isn_end,
						sizeof(isn_T) * arg_isn_count);
		    mch_memmove(((isn_T *)cctx->ctx_instr.ga_data)
					      + expr_isn_start + arg_isn_count,
					  isn, sizeof(isn_T) * expr_isn_count);
		    vim_free(isn);

		    typep = ((type2_T *)stack->ga_data) + type_idx_start;
		    type = typep->type_curr;
		    decl_type = typep->type_decl;
		    mch_memmove(((type2_T *)stack->ga_data) + type_idx_start,
			      ((type2_T *)stack->ga_data) + type_idx_start + 1,
			      sizeof(type2_T)
				       * (stack->ga_len - type_idx_start - 1));
		    typep = ((type2_T *)stack->ga_data) + stack->ga_len - 1;
		    typep->type_curr = type;
		    typep->type_decl = decl_type;
		}

		type = get_type_on_stack(cctx, 0);
		if (generate_PCALL(cctx, argcount, p - 2, type, FALSE) == FAIL)
		    return FAIL;
	    }
	    else
	    {
		// method call:  list->method()
		p = *arg;
		if (!eval_isnamec1(*p))
		{
		    semsg(_(e_trailing_characters_str), pstart);
		    return FAIL;
		}
		if (ASCII_ISALPHA(*p) && p[1] == ':')
		    p += 2;
		for ( ; eval_isnamec(*p); ++p)
		    ;
		if (*p != '(')
		{
		    semsg(_(e_missing_parenthesis_str), *arg);
		    return FAIL;
		}
		if (compile_call(arg, p - *arg, cctx, ppconst, 1) == FAIL)
		    return FAIL;
	    }
	    if (keeping_dict)
	    {
		keeping_dict = FALSE;
		if (generate_instr(cctx, ISN_CLEARDICT) == NULL)
		    return FAIL;
	    }
	}
	else if (**arg == '[')
	{
	    int		is_slice = FALSE;

	    // list index: list[123]
	    // dict member: dict[key]
	    // string index: text[123]
	    // blob index: blob[123]
	    if (generate_ppconst(cctx, ppconst) == FAIL)
		return FAIL;
	    ppconst->pp_is_const = FALSE;

	    ++p;
	    if (may_get_next_line_error(p, arg, cctx) == FAIL)
		return FAIL;
	    if (**arg == ':')
	    {
		// missing first index is equal to zero
		generate_PUSHNR(cctx, 0);
	    }
	    else
	    {
		if (compile_expr0(arg, cctx) == FAIL)
		    return FAIL;
		if (**arg == ':')
		{
		    semsg(_(e_white_space_required_before_and_after_str_at_str),
								    ":", *arg);
		    return FAIL;
		}
		if (may_get_next_line_error(*arg, arg, cctx) == FAIL)
		    return FAIL;
		*arg = skipwhite(*arg);
	    }
	    if (**arg == ':')
	    {
		is_slice = TRUE;
		++*arg;
		if (!IS_WHITE_OR_NUL(**arg) && **arg != ']')
		{
		    semsg(_(e_white_space_required_before_and_after_str_at_str),
								    ":", *arg);
		    return FAIL;
		}
		if (may_get_next_line_error(*arg, arg, cctx) == FAIL)
		    return FAIL;
		if (**arg == ']')
		    // missing second index is equal to end of string
		    generate_PUSHNR(cctx, -1);
		else
		{
		    if (compile_expr0(arg, cctx) == FAIL)
			return FAIL;
		    if (may_get_next_line_error(*arg, arg, cctx) == FAIL)
			return FAIL;
		    *arg = skipwhite(*arg);
		}
	    }

	    if (**arg != ']')
	    {
		emsg(_(e_missing_closing_square_brace));
		return FAIL;
	    }
	    *arg = *arg + 1;

	    if (keeping_dict)
	    {
		keeping_dict = FALSE;
		if (generate_instr(cctx, ISN_CLEARDICT) == NULL)
		    return FAIL;
	    }
	    if (compile_member(is_slice, &keeping_dict, cctx) == FAIL)
		return FAIL;
	}
	else if (*p == '.' && p[1] != '.')
	{
	    // dictionary member: dict.name
	    if (generate_ppconst(cctx, ppconst) == FAIL)
		return FAIL;
	    ppconst->pp_is_const = FALSE;

	    *arg = p + 1;
	    if (IS_WHITE_OR_NUL(**arg))
	    {
		emsg(_(e_missing_name_after_dot));
		return FAIL;
	    }
	    p = *arg;
	    if (eval_isdictc(*p))
		while (eval_isnamec(*p))
		    MB_PTR_ADV(p);
	    if (p == *arg)
	    {
		semsg(_(e_syntax_error_at_str), *arg);
		return FAIL;
	    }
	    if (keeping_dict && generate_instr(cctx, ISN_CLEARDICT) == NULL)
		return FAIL;
	    if (generate_STRINGMEMBER(cctx, *arg, p - *arg) == FAIL)
		return FAIL;
	    keeping_dict = TRUE;
	    *arg = p;
	}
	else
	    break;
    }

    // Turn "dict.Func" into a partial for "Func" bound to "dict".
    // This needs to be done at runtime to be able to check the type.
    if (keeping_dict && generate_instr(cctx, ISN_USEDICT) == NULL)
	return FAIL;

    return OK;
}