new_scope(cctx_T *cctx, scopetype_T type)
{
    scope_T *scope = ALLOC_CLEAR_ONE(scope_T);

    if (scope == NULL)
	return NULL;
    scope->se_outer = cctx->ctx_scope;
    cctx->ctx_scope = scope;
    scope->se_type = type;
    scope->se_local_count = cctx->ctx_locals.ga_len;
    return scope;
}