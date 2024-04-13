compile_if(char_u *arg, cctx_T *cctx)
{
    char_u	*p = arg;
    garray_T	*instr = &cctx->ctx_instr;
    int		instr_count = instr->ga_len;
    scope_T	*scope;
    skip_T	skip_save = cctx->ctx_skip;
    ppconst_T	ppconst;

    CLEAR_FIELD(ppconst);
    if (compile_expr1(&p, cctx, &ppconst) == FAIL)
    {
	clear_ppconst(&ppconst);
	return NULL;
    }
    if (!ends_excmd2(arg, skipwhite(p)))
    {
	semsg(_(e_trailing_characters_str), p);
	return NULL;
    }
    if (cctx->ctx_skip == SKIP_YES)
	clear_ppconst(&ppconst);
    else if (instr->ga_len == instr_count && ppconst.pp_used == 1)
    {
	int error = FALSE;
	int v;

	// The expression results in a constant.
	v = tv_get_bool_chk(&ppconst.pp_tv[0], &error);
	clear_ppconst(&ppconst);
	if (error)
	    return NULL;
	cctx->ctx_skip = v ? SKIP_NOT : SKIP_YES;
    }
    else
    {
	// Not a constant, generate instructions for the expression.
	cctx->ctx_skip = SKIP_UNKNOWN;
	if (generate_ppconst(cctx, &ppconst) == FAIL)
	    return NULL;
	if (bool_on_stack(cctx) == FAIL)
	    return NULL;
    }

    // CMDMOD_REV must come before the jump
    generate_undo_cmdmods(cctx);

    scope = new_scope(cctx, IF_SCOPE);
    if (scope == NULL)
	return NULL;
    scope->se_skip_save = skip_save;
    // "is_had_return" will be reset if any block does not end in :return
    scope->se_u.se_if.is_had_return = TRUE;

    if (cctx->ctx_skip == SKIP_UNKNOWN)
    {
	// "where" is set when ":elseif", "else" or ":endif" is found
	scope->se_u.se_if.is_if_label = instr->ga_len;
	generate_JUMP(cctx, JUMP_IF_FALSE, 0);
    }
    else
	scope->se_u.se_if.is_if_label = -1;

#ifdef FEAT_PROFILE
    if (cctx->ctx_compile_type == CT_PROFILE && cctx->ctx_skip == SKIP_YES
						      && skip_save != SKIP_YES)
    {
	// generated a profile start, need to generate a profile end, since it
	// won't be done after returning
	cctx->ctx_skip = SKIP_NOT;
	generate_instr(cctx, ISN_PROF_END);
	cctx->ctx_skip = SKIP_YES;
    }
#endif

    return p;
}