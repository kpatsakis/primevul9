static void zstd_done(ZSTD_CCtx *cctx)
{
    if (cctx) ZSTD_freeCCtx(cctx);
}