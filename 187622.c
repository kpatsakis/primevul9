EXPORTED void simple_hdr(struct transaction_t *txn,
                         const char *name, const char *value, ...)
{
    struct buf buf = BUF_INITIALIZER;
    va_list args;

    va_start(args, value);
    buf_vprintf(&buf, value, args);
    va_end(args);

    syslog(LOG_DEBUG, "simple_hdr(%s: %s)", name, buf_cstring(&buf));

    if (txn->flags.ver == VER_2) {
        http2_add_header(txn, name, &buf);
    }
    else {
        prot_printf(txn->conn->pout, "%c%s: ", toupper(name[0]), name+1);
        prot_puts(txn->conn->pout, buf_cstring(&buf));
        prot_puts(txn->conn->pout, "\r\n");

        buf_free(&buf);
    }
}