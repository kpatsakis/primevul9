HIDDEN int zlib_compress(struct transaction_t *txn, unsigned flags,
                         const char *buf, unsigned len)
{
    z_stream *zstrm = txn->zstrm;
    unsigned flush;

    if (flags & COMPRESS_START) deflateReset(zstrm);

    if (txn->ws_ctx) flush = Z_SYNC_FLUSH;
    else {
        /* Only flush for static content or on last (zero-length) chunk */
        if (flags & COMPRESS_END) flush = Z_FINISH;
        else flush = Z_NO_FLUSH;
    }

    zstrm->next_in = (Bytef *) buf;
    zstrm->avail_in = len;

    buf_ensure(&txn->zbuf, deflateBound(zstrm, zstrm->avail_in));
    buf_reset(&txn->zbuf);

    do {
        int zr;

        if (!zstrm->avail_out) {
            unsigned pending;

            zr = deflatePending(zstrm, &pending, Z_NULL);
            if (zr != Z_OK) {
                /* something went wrong */
                syslog(LOG_ERR, "zlib deflate error: %d %s", zr, zstrm->msg);
                return -1;
            }

            buf_ensure(&txn->zbuf, pending);
        }

        zstrm->next_out = (Bytef *) txn->zbuf.s + txn->zbuf.len;
        zstrm->avail_out = txn->zbuf.alloc - txn->zbuf.len;

        zr = deflate(zstrm, flush);
        if (!(zr == Z_OK || zr == Z_STREAM_END || zr == Z_BUF_ERROR)) {
            /* something went wrong */
            syslog(LOG_ERR, "zlib deflate error: %d %s", zr, zstrm->msg);
            return -1;
        }

        txn->zbuf.len = txn->zbuf.alloc - zstrm->avail_out;

    } while (!zstrm->avail_out);

    return 0;
}