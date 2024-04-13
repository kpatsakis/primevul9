static int check_namespace(struct transaction_t *txn)
{
    int i;
    const char **hdr, *query = URI_QUERY(txn->req_uri);
    const struct namespace_t *namespace;
    const struct method_t *meth_t;

    /* Find the namespace of the requested resource */
    for (i = 0; http_namespaces[i]; i++) {
        const char *path = txn->req_uri->path;
        size_t len;

        /* Skip disabled namespaces */
        if (!http_namespaces[i]->enabled) continue;

        /* Handle any /.well-known/ bootstrapping */
        if (http_namespaces[i]->well_known) {
            len = strlen(http_namespaces[i]->well_known);
            if (!strncmp(path, http_namespaces[i]->well_known, len) &&
                (!path[len] || path[len] == '/')) {

                hdr = spool_getheader(txn->req_hdrs, ":authority");
                buf_reset(&txn->buf);
                buf_printf(&txn->buf, "%s://%s",
                           https ? "https" : "http", hdr[0]);
                buf_appendcstr(&txn->buf, http_namespaces[i]->prefix);
                buf_appendcstr(&txn->buf, path + len);
                if (query) buf_printf(&txn->buf, "?%s", query);
                txn->location = buf_cstring(&txn->buf);

                return HTTP_MOVED;
            }
        }

        /* See if the prefix matches - terminated with NUL or '/' */
        len = strlen(http_namespaces[i]->prefix);
        if (!strncmp(path, http_namespaces[i]->prefix, len) &&
            (!path[len] || (path[len] == '/') || !strcmp(path, "*"))) {
            break;
        }
    }
    if ((namespace = http_namespaces[i])) {
        txn->req_tgt.namespace = namespace;
        txn->req_tgt.allow = namespace->allow;

        /* Check if method is supported in this namespace */
        meth_t = &namespace->methods[txn->meth];
        if (!meth_t->proc) return HTTP_NOT_ALLOWED;

        /* Check if method expects a body */
        else if ((http_methods[txn->meth].flags & METH_NOBODY) &&
                 (txn->req_body.framing != FRAMING_LENGTH ||
                  /* XXX  Will break if client sends just a last-chunk */
                  txn->req_body.len)) {
            return HTTP_BAD_MEDIATYPE;
        }
    } else {
        /* XXX  Should never get here */
        return HTTP_SERVER_ERROR;
    }

    /* See if this namespace whitelists auth schemes */
    if (namespace->auth_schemes) {
        avail_auth_schemes = (namespace->auth_schemes & avail_auth_schemes);

        /* Bearer auth must be advertised and supported by the namespace */
        if ((namespace->auth_schemes & AUTH_BEARER) && namespace->bearer) {
            avail_auth_schemes |= AUTH_BEARER;
        }
    }

    return 0;
}