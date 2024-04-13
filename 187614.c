static int check_method(struct transaction_t *txn)
{
    const char **hdr;
    struct request_line_t *req_line = &txn->req_line;

    if (txn->flags.redirect) return 0;

    /* Check for HTTP method override */
    if (!strcmp(req_line->meth, "POST") &&
        (hdr = spool_getheader(txn->req_hdrs, "X-HTTP-Method-Override"))) {
        txn->flags.override = 1;
        req_line->meth = (char *) hdr[0];
    }

    /* Check Method against our list of known methods */
    for (txn->meth = 0; (txn->meth < METH_UNKNOWN) &&
             strcmp(http_methods[txn->meth].name, req_line->meth);
         txn->meth++);

    if (txn->meth == METH_UNKNOWN) return HTTP_NOT_IMPLEMENTED;

    return 0;
}