EXPORTED void comma_list_hdr(struct transaction_t *txn, const char *name,
                             const char *vals[], unsigned flags, ...)
{
    struct buf buf = BUF_INITIALIZER;
    va_list args;

    va_start(args, flags);

    comma_list_body(&buf, vals, flags, args);

    va_end(args);

    simple_hdr(txn, name, buf_cstring(&buf));

    buf_free(&buf);
}