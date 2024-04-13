static int meth_propfind_root(struct transaction_t *txn,
                              void *params __attribute__((unused)))
{
    assert(txn);

#ifdef WITH_DAV
    /* Apple iCal and Evolution both check "/" */
    if (!strcmp(txn->req_uri->path, "/") ||
        !strcmp(txn->req_uri->path, "/dav/")) {
        /* Array of known "live" properties */
        const struct prop_entry root_props[] = {

            /* WebDAV ACL (RFC 3744) properties */
            { "principal-collection-set", NS_DAV, PROP_COLLECTION,
              propfind_princolset, NULL, NULL },

            /* WebDAV Current Principal (RFC 5397) properties */
            { "current-user-principal", NS_DAV, PROP_COLLECTION,
              propfind_curprin, NULL, NULL },

            { NULL, 0, 0, NULL, NULL, NULL }
        };

        struct meth_params root_params = {
            .propfind = { DAV_FINITE_DEPTH, root_props }
        };

        /* Make a working copy of target path */
        strlcpy(txn->req_tgt.path, txn->req_uri->path,
                sizeof(txn->req_tgt.path));
        txn->req_tgt.tail = txn->req_tgt.path + strlen(txn->req_tgt.path);

        txn->req_tgt.allow |= ALLOW_DAV;
        return meth_propfind(txn, &root_params);
    }
#endif /* WITH_DAV */

    return HTTP_NOT_ALLOWED;
}