EXPORTED void accept_patch_hdr(struct transaction_t *txn,
                               const struct patch_doc_t *patch)
{
    struct buf buf = BUF_INITIALIZER;
    const char *sep = "";
    int i;

    for (i = 0; patch[i].format; i++) {
        buf_appendcstr(&buf, sep);
        buf_appendcstr(&buf, patch[i].format);
        sep = ", ";
    }

    simple_hdr(txn, "Accept-Patch", buf_cstring(&buf));

    buf_free(&buf);
}