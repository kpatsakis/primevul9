EXPORTED void transaction_free(struct transaction_t *txn)
{
    transaction_reset(txn);

    ws_end_channel(txn->ws_ctx);
    http2_end_stream(txn->strm_ctx);

    zlib_done(txn->zstrm);
    zstd_done(txn->zstd);
    brotli_done(txn->brotli);

    buf_free(&txn->req_body.payload);
    buf_free(&txn->resp_body.payload);
    buf_free(&txn->zbuf);
    buf_free(&txn->buf);
}