apr_status_t h2_request_end_headers(h2_request *req, apr_pool_t *pool, int eos, size_t raw_bytes)
{
    const char *s;
    
    /* rfc7540, ch. 8.1.2.3:
     * - if we have :authority, it overrides any Host header 
     * - :authority MUST be ommited when converting h1->h2, so we
     *   might get a stream without, but then Host needs to be there */
    if (!req->authority) {
        const char *host = apr_table_get(req->headers, "Host");
        if (!host) {
            return APR_BADARG;
        }
        req->authority = host;
    }
    else {
        apr_table_setn(req->headers, "Host", req->authority);
    }

    s = apr_table_get(req->headers, "Content-Length");
    if (!s) {
        /* HTTP/2 does not need a Content-Length for framing, but our
         * internal request processing is used to HTTP/1.1, so we
         * need to either add a Content-Length or a Transfer-Encoding
         * if any content can be expected. */
        if (!eos) {
            /* We have not seen a content-length and have no eos,
             * simulate a chunked encoding for our HTTP/1.1 infrastructure,
             * in case we have "H2SerializeHeaders on" here
             */
            req->chunked = 1;
            apr_table_mergen(req->headers, "Transfer-Encoding", "chunked");
        }
        else if (apr_table_get(req->headers, "Content-Type")) {
            /* If we have a content-type, but already seen eos, no more
             * data will come. Signal a zero content length explicitly.
             */
            apr_table_setn(req->headers, "Content-Length", "0");
        }
    }
    req->raw_bytes += raw_bytes;
    
    return APR_SUCCESS;
}