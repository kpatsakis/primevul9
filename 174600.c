free_locals(cctx_T *cctx)
{
    unwind_locals(cctx, 0);
    ga_clear(&cctx->ctx_locals);
}