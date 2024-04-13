compile_continue(char_u *arg, cctx_T *cctx)
{
    scope_T	*scope = cctx->ctx_scope;
    int		try_scopes = 0;
    int		loop_label;

    for (;;)
    {
	if (scope == NULL)
	{
	    emsg(_(e_continue_without_while_or_for));
	    return NULL;
	}
	if (scope->se_type == FOR_SCOPE)
	{
	    loop_label = scope->se_u.se_for.fs_top_label;
	    break;
	}
	if (scope->se_type == WHILE_SCOPE)
	{
	    loop_label = scope->se_u.se_while.ws_top_label;
	    break;
	}
	if (scope->se_type == TRY_SCOPE)
	    ++try_scopes;
	scope = scope->se_outer;
    }

    if (try_scopes > 0)
	// Inside one or more try/catch blocks we first need to jump to the
	// "finally" or "endtry" to cleanup.
	generate_TRYCONT(cctx, try_scopes, loop_label);
    else
	// Jump back to the FOR or WHILE instruction.
	generate_JUMP(cctx, JUMP_ALWAYS, loop_label);

    return arg;
}