compile_return(char_u *arg, int check_return_type, int legacy, cctx_T *cctx)
{
    char_u	*p = arg;
    type_T	*stack_type;

    if (*p != NUL && *p != '|' && *p != '\n')
    {
	// For a lambda, "return expr" is always used, also when "expr" results
	// in a void.
	if (cctx->ctx_ufunc->uf_ret_type->tt_type == VAR_VOID
		&& (cctx->ctx_ufunc->uf_flags & FC_LAMBDA) == 0)
	{
	    emsg(_(e_returning_value_in_function_without_return_type));
	    return NULL;
	}
	if (legacy)
	{
	    int save_flags = cmdmod.cmod_flags;

	    generate_LEGACY_EVAL(cctx, p);
	    if (need_type(&t_any, cctx->ctx_ufunc->uf_ret_type, -1,
						0, cctx, FALSE, FALSE) == FAIL)
		return NULL;
	    cmdmod.cmod_flags |= CMOD_LEGACY;
	    (void)skip_expr(&p, NULL);
	    cmdmod.cmod_flags = save_flags;
	}
	else
	{
	    // compile return argument into instructions
	    if (compile_expr0(&p, cctx) == FAIL)
		return NULL;
	}

	if (cctx->ctx_skip != SKIP_YES)
	{
	    // "check_return_type" with uf_ret_type set to &t_unknown is used
	    // for an inline function without a specified return type.  Set the
	    // return type here.
	    stack_type = get_type_on_stack(cctx, 0);
	    if ((check_return_type && (cctx->ctx_ufunc->uf_ret_type == NULL
				|| cctx->ctx_ufunc->uf_ret_type == &t_unknown))
		    || (!check_return_type
				&& cctx->ctx_ufunc->uf_ret_type == &t_unknown))
	    {
		cctx->ctx_ufunc->uf_ret_type = stack_type;
	    }
	    else
	    {
		if (need_type(stack_type, cctx->ctx_ufunc->uf_ret_type, -1,
						0, cctx, FALSE, FALSE) == FAIL)
		    return NULL;
	    }
	}
    }
    else
    {
	// "check_return_type" cannot be TRUE, only used for a lambda which
	// always has an argument.
	if (cctx->ctx_ufunc->uf_ret_type->tt_type != VAR_VOID
		&& cctx->ctx_ufunc->uf_ret_type->tt_type != VAR_UNKNOWN)
	{
	    emsg(_(e_missing_return_value));
	    return NULL;
	}

	// No argument, return zero.
	generate_PUSHNR(cctx, 0);
    }

    // Undo any command modifiers.
    generate_undo_cmdmods(cctx);

    if (cctx->ctx_skip != SKIP_YES && generate_instr(cctx, ISN_RETURN) == NULL)
	return NULL;

    // "return val | endif" is possible
    return skipwhite(p);
}