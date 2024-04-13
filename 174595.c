compile_break(char_u *arg, cctx_T *cctx)
{
    scope_T	*scope = cctx->ctx_scope;
    int		try_scopes = 0;
    endlabel_T	**el;

    for (;;)
    {
	if (scope == NULL)
	{
	    emsg(_(e_break_without_while_or_for));
	    return NULL;
	}
	if (scope->se_type == FOR_SCOPE)
	{
	    el = &scope->se_u.se_for.fs_end_label;
	    break;
	}
	if (scope->se_type == WHILE_SCOPE)
	{
	    el = &scope->se_u.se_while.ws_end_label;
	    break;
	}
	if (scope->se_type == TRY_SCOPE)
	    ++try_scopes;
	scope = scope->se_outer;
    }

    if (try_scopes > 0)
	// Inside one or more try/catch blocks we first need to jump to the
	// "finally" or "endtry" to cleanup.  Then come to the next JUMP
	// intruction, which we don't know the index of yet.
	generate_TRYCONT(cctx, try_scopes, cctx->ctx_instr.ga_len + 1);

    // Jump to the end of the FOR or WHILE loop.  The instruction index will be
    // filled in later.
    if (compile_jump_to_end(el, JUMP_ALWAYS, cctx) == FAIL)
	return FAIL;

    return arg;
}