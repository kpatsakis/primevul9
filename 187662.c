EXPORTED int meth_trace(struct transaction_t *txn, void *params)
{
    parse_path_t parse_path = (parse_path_t) params;
    const char **hdr;
    unsigned long max_fwd = -1;
    struct buf *msg = &txn->resp_body.payload;

    /* Response should not be cached */
    txn->flags.cc |= CC_NOCACHE;

    /* Make sure method is allowed */
    if (!(txn->req_tgt.allow & ALLOW_TRACE)) return HTTP_NOT_ALLOWED;

    if ((hdr = spool_getheader(txn->req_hdrs, "Max-Forwards"))) {
        max_fwd = strtoul(hdr[0], NULL, 10);
    }

    if (max_fwd && parse_path) {
        /* Parse the path */
        int r;

        if ((r = parse_path(txn->req_uri->path,
                            &txn->req_tgt, &txn->error.desc))) return r;

        if (txn->req_tgt.mbentry && txn->req_tgt.mbentry->server) {
            /* Remote mailbox */
            struct backend *be;

            be = proxy_findserver(txn->req_tgt.mbentry->server,
                                  &http_protocol, httpd_userid,
                                  &backend_cached, NULL, NULL, httpd_in);
            if (!be) return HTTP_UNAVAILABLE;

            return http_pipe_req_resp(be, txn);
        }

        /* Local mailbox */
    }

    /* Echo the request back to the client as a message/http:
     *
     * - Piece the Request-line back together
     * - Use all non-sensitive cached headers from client
     */
    buf_reset(msg);
    buf_printf(msg, "TRACE %s %s\r\n", txn->req_line.uri, txn->req_line.ver);
    spool_enum_hdrcache(txn->req_hdrs, &trace_cachehdr, msg);
    buf_appendcstr(msg, "\r\n");

    txn->resp_body.type = "message/http";
    txn->resp_body.len = buf_len(msg);

    write_body(HTTP_OK, txn, buf_cstring(msg), buf_len(msg));

    return 0;
}