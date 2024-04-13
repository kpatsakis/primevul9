HIDDEN int meth_connect(struct transaction_t *txn, void *params)
{
    struct connect_params *cparams = (struct connect_params *) params;

    /* Bootstrap WebSockets over HTTP/2, if requested */
    if ((txn->flags.ver != VER_2) ||
        !ws_enabled() || !cparams || !cparams->endpoint) {
        return HTTP_NOT_IMPLEMENTED;
    }

    if (strcmp(txn->req_uri->path, cparams->endpoint)) return HTTP_NOT_ALLOWED;

    if (!(txn->flags.upgrade & UPGRADE_WS)) {
        txn->error.desc = "Missing/unsupported :protocol value ";
        return HTTP_BAD_REQUEST;
    }

    int ret = ws_start_channel(txn, cparams->subprotocol, cparams->data_cb);

    return (ret == HTTP_UPGRADE) ? HTTP_BAD_REQUEST : ret;
}