unwind_locals(cctx_T *cctx, int new_top)
{
    if (cctx->ctx_locals.ga_len > new_top)
    {
	int	idx;
	lvar_T	*lvar;

	for (idx = new_top; idx < cctx->ctx_locals.ga_len; ++idx)
	{
	    lvar = ((lvar_T *)cctx->ctx_locals.ga_data) + idx;
	    vim_free(lvar->lv_name);
	}
    }
    cctx->ctx_locals.ga_len = new_top;
}