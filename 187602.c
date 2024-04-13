static int zstd_compress(struct transaction_t *txn,
                         unsigned flags, const char *buf, unsigned len)
{
    /* Only flush for static content or on last (zero-length) chunk */
    ZSTD_EndDirective mode = (flags & COMPRESS_END) ? ZSTD_e_end : ZSTD_e_flush;
    ZSTD_inBuffer input = { buf, len, 0 };
    ZSTD_CCtx *cctx = txn->zstd;
    size_t remaining;

    if (flags & COMPRESS_START) ZSTD_CCtx_reset(cctx, ZSTD_reset_session_only);

    buf_ensure(&txn->zbuf, ZSTD_compressBound(len));
    buf_reset(&txn->zbuf);

    ZSTD_outBuffer output = { txn->zbuf.s, txn->zbuf.alloc, 0 };
    do {
        remaining = ZSTD_compressStream2(cctx, &output, &input, mode);

        if (ZSTD_isError(remaining)) {
            syslog(LOG_ERR, "Zstandard: %s",
                   ZSTD_getErrorString(ZSTD_getErrorCode(remaining)));
            return -1;
        }
    } while (remaining || (input.pos != input.size));

    buf_truncate(&txn->zbuf, output.pos);

    return 0;
}