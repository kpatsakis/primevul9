compile_while(char_u *arg, cctx_T *cctx)
{
    char_u	*p = arg;
    scope_T	*scope;

    scope = new_scope(cctx, WHILE_SCOPE);
    if (scope == NULL)
	return NULL;

    // "endwhile" jumps back here, one before when profiling or using cmdmods
    scope->se_u.se_while.ws_top_label = current_instr_idx(cctx);

    // compile "expr"
    if (compile_expr0(&p, cctx) == FAIL)
	return NULL;

    if (!ends_excmd2(arg, skipwhite(p)))
    {
	semsg(_(e_trailing_characters_str), p);
	return NULL;
    }

    if (cctx->ctx_skip != SKIP_YES)
    {
	if (bool_on_stack(cctx) == FAIL)
	    return FAIL;

	// CMDMOD_REV must come before the jump
	generate_undo_cmdmods(cctx);

	// "while_end" is set when ":endwhile" is found
	if (compile_jump_to_end(&scope->se_u.se_while.ws_end_label,
						  JUMP_IF_FALSE, cctx) == FAIL)
	    return FAIL;
    }

    return p;
}