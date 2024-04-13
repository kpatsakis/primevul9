EXPORTED int check_precond(struct transaction_t *txn,
                           const char *etag, time_t lastmod)
{
    hdrcache_t hdrcache = txn->req_hdrs;
    const char **hdr;
    time_t since = 0;

    /* Step 1 */
    if ((hdr = spool_getheader(hdrcache, "If-Match"))) {
        if (!etag_match(hdr, etag)) return HTTP_PRECOND_FAILED;

        /* Continue to step 3 */
    }

    /* Step 2 */
    else if ((hdr = spool_getheader(hdrcache, "If-Unmodified-Since"))) {
        if (time_from_rfc5322(hdr[0], &since, DATETIME_FULL) < 0)
            return HTTP_BAD_REQUEST;

        if (lastmod > since) return HTTP_PRECOND_FAILED;

        /* Continue to step 3 */
    }

    /* Step 3 */
    if ((hdr = spool_getheader(hdrcache, "If-None-Match"))) {
        if (etag_match(hdr, etag)) {
            if (txn->meth == METH_GET || txn->meth == METH_HEAD)
                return HTTP_NOT_MODIFIED;
            else
                return HTTP_PRECOND_FAILED;
        }

        /* Continue to step 5 */
    }

    /* Step 4 */
    else if ((txn->meth == METH_GET || txn->meth == METH_HEAD) &&
             (hdr = spool_getheader(hdrcache, "If-Modified-Since"))) {
        if (time_from_rfc5322(hdr[0], &since, DATETIME_FULL) < 0)
            return HTTP_BAD_REQUEST;

        if (lastmod <= since) return HTTP_NOT_MODIFIED;

        /* Continue to step 5 */
    }

    /* Step 5 */
    if (txn->flags.ranges &&  /* Only if we support Range requests */
        txn->meth == METH_GET && (hdr = spool_getheader(hdrcache, "Range"))) {

        if ((hdr = spool_getheader(hdrcache, "If-Range"))) {
            time_from_rfc5322(hdr[0], &since, DATETIME_FULL); /* error OK here, could be an etag */
        }

        /* Only process Range if If-Range isn't present or validator matches */
        if (!hdr || (since && (lastmod <= since)) || !etagcmp(hdr[0], etag))
            return HTTP_PARTIAL;
    }

    /* Step 6 */
    return HTTP_OK;
}