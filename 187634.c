EXPORTED int end_resp_headers(struct transaction_t *txn, long code)
{
    int r = 0;

    if (txn->flags.ver == VER_2) {
        r = http2_end_headers(txn, code);
    }
    else {
        /* CRLF terminating the header block */
        prot_puts(txn->conn->pout, "\r\n");
    }

    return r;
}