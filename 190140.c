void am_set_cache_control_headers(request_rec *r)
{
    /* Send Cache-Control header to ensure that:
     * - no proxy in the path caches content inside this location (private),
     * - user agent have to revalidate content on server (must-revalidate).
     *
     * But never prohibit specifically any user agent to cache or store content
     *
     * Setting the headers in err_headers_out ensures that they will be
     * sent for all responses.
     */
    apr_table_setn(r->err_headers_out,
                   "Cache-Control", "private, must-revalidate");
}