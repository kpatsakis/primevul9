EXPORTED void write_multipart_body(long code, struct transaction_t *txn,
                                   const char *buf, unsigned len,
                                   const char *part_headers)
{
    static char boundary[100];
    struct buf *body = &txn->resp_body.payload;

    if (code) {
        const char *preamble =
            "This is a message with multiple parts in MIME format.\r\n";

        txn->flags.mime = 1;

        /* Create multipart boundary */
        snprintf(boundary, sizeof(boundary), "%s-%ld-%ld-%ld",
                 *spool_getheader(txn->req_hdrs, ":authority"),
                 (long) getpid(), (long) time(0), (long) rand());

        /* Create Content-Type w/ boundary */
        assert(!buf_len(&txn->buf));
        buf_printf(&txn->buf, "%s; boundary=\"%s\"",
                   txn->resp_body.type, boundary);
        txn->resp_body.type = buf_cstring(&txn->buf);

        /* Setup for chunked response and begin multipart */
        txn->flags.te |= TE_CHUNKED;
        if (!buf) {
            buf = preamble;
            len = strlen(preamble);
        }
        write_body(code, txn, buf, len);
    }
    else if (len) {
        /* Output delimiter and MIME part-headers */
        buf_reset(body);
        buf_printf(body, "\r\n--%s\r\n", boundary);
        buf_printf(body, "Content-Type: %s\r\n", txn->resp_body.type);
        if (txn->resp_body.range) {
            buf_printf(body, "Content-Range: bytes %lu-%lu/%lu\r\n",
                       txn->resp_body.range->first,
                       txn->resp_body.range->last,
                       txn->resp_body.len);
        }
        buf_printf(body, "Content-Length: %d\r\n", len);
        if (part_headers) {
            buf_appendcstr(body, part_headers);
        }
        buf_appendcstr(body, "\r\n");
        write_body(0, txn, buf_cstring(body), buf_len(body));

        /* Output body-part data */
        write_body(0, txn, buf, len);
    }
    else {
        const char *epilogue = "\r\nEnd of MIME multipart body.\r\n";

        /* Output close-delimiter and epilogue */
        buf_reset(body);
        buf_printf(body, "\r\n--%s--\r\n%s", boundary, epilogue);
        write_body(0, txn, buf_cstring(body), buf_len(body));

        /* End of output */
        write_body(0, txn, NULL, 0);
    }
}