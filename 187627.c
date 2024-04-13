EXPORTED int meth_options(struct transaction_t *txn, void *params)
{
    parse_path_t parse_path = (parse_path_t) params;
    int r, i;

    /* Response should not be cached */
    txn->flags.cc |= CC_NOCACHE;

    /* Response doesn't have a body, so no Vary */
    txn->flags.vary = 0;

    /* Special case "*" - show all features/methods available on server */
    if (!strcmp(txn->req_uri->path, "*")) {
        for (i = 0; http_namespaces[i]; i++) {
            if (http_namespaces[i]->enabled)
                txn->req_tgt.allow |= http_namespaces[i]->allow;
        }

        if (ws_enabled() && (txn->flags.ver == VER_2)) {
            /* CONNECT allowed for bootstrapping WebSocket over HTTP/2 */
            txn->req_tgt.allow |= ALLOW_CONNECT;
        }
    }
    else {
        if (parse_path) {
            /* Parse the path */
            r = parse_path(txn->req_uri->path, &txn->req_tgt, &txn->error.desc);
            if (r) return r;
        }
        else if (!strcmp(txn->req_uri->path, "/") &&
                 ws_enabled() && (txn->flags.ver == VER_2)) {
            /* WS 'echo' endpoint */
            txn->req_tgt.allow |= ALLOW_CONNECT;
        }

        if (txn->flags.cors) {
            const char **hdr =
                spool_getheader(txn->req_hdrs, "Access-Control-Request-Method");

            if (hdr) {
                /* CORS preflight request */
                unsigned meth;

                txn->flags.cors = CORS_PREFLIGHT;

                /* Check Method against our list of known methods */
                for (meth = 0; (meth < METH_UNKNOWN) &&
                         strcmp(http_methods[meth].name, hdr[0]); meth++);

                if (meth == METH_UNKNOWN) txn->flags.cors = 0;
                else {
                    /* Check Method against those supported by the resource */
                    if (!txn->req_tgt.namespace->methods[meth].proc)
                        txn->flags.cors = 0;
                }
            }
        }
    }

    response_header(HTTP_OK, txn);
    return 0;
}