static int brotli_compress(struct transaction_t *txn,
                           unsigned flags, const char *buf, unsigned len)
{
    /* Only flush for static content or on last (zero-length) chunk */
    unsigned op = (flags & COMPRESS_END) ?
        BROTLI_OPERATION_FINISH : BROTLI_OPERATION_FLUSH;
    BrotliEncoderState *brotli = txn->brotli;
    const uint8_t *next_in = (const uint8_t *) buf;
    size_t avail_in = (size_t) len;

    buf_ensure(&txn->zbuf, BrotliEncoderMaxCompressedSize(avail_in));
    buf_reset(&txn->zbuf);

    do {
        uint8_t *next_out = (uint8_t *) txn->zbuf.s + txn->zbuf.len;
        size_t avail_out = txn->zbuf.alloc - txn->zbuf.len;

        if (!BrotliEncoderCompressStream(brotli, op,
                                         &avail_in, &next_in,
                                         &avail_out, &next_out, NULL)) {
            syslog(LOG_ERR, "Brotli: Error while compressing data");
            return -1;
        }

        txn->zbuf.len = txn->zbuf.alloc - avail_out;
    } while (avail_in || BrotliEncoderHasMoreOutput(brotli));

    if (BrotliEncoderIsFinished(brotli)) {
        BrotliEncoderDestroyInstance(brotli);
        txn->brotli = brotli_init();
    }

    return 0;
}