drop_scope(cctx_T *cctx)
{
    scope_T *scope = cctx->ctx_scope;

    if (scope == NULL)
    {
	iemsg("calling drop_scope() without a scope");
	return;
    }
    cctx->ctx_scope = scope->se_outer;
    switch (scope->se_type)
    {
	case IF_SCOPE:
	    compile_free_jump_to_end(&scope->se_u.se_if.is_end_label); break;
	case FOR_SCOPE:
	    compile_free_jump_to_end(&scope->se_u.se_for.fs_end_label); break;
	case WHILE_SCOPE:
	    compile_free_jump_to_end(&scope->se_u.se_while.ws_end_label); break;
	case TRY_SCOPE:
	    compile_free_jump_to_end(&scope->se_u.se_try.ts_end_label); break;
	case NO_SCOPE:
	case BLOCK_SCOPE:
	    break;
    }
    vim_free(scope);
}