EXPORTED void begin_resp_headers(struct transaction_t *txn, long code)
{
    if (txn->flags.ver == VER_2) {
        http2_begin_headers(txn);
        if (code) simple_hdr(txn, ":status", "%.3s", error_message(code));
    }
    else if (code) prot_printf(txn->conn->pout, "%s\r\n",
                               http_statusline(txn->flags.ver, code));
}