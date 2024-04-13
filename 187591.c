HIDDEN void *zstd_init()
{
    ZSTD_CCtx *cctx = ZSTD_createCCtx();

    if (cctx) {
        ZSTD_CCtx_setParameter(cctx, ZSTD_c_compressionLevel,
                               ZSTD_CLEVEL_DEFAULT);
        ZSTD_CCtx_setParameter(cctx, ZSTD_c_checksumFlag, 1);
    }

    return cctx;
}