apr_status_t h2_request_add_header(h2_request *req, apr_pool_t *pool, 
                                   const char *name, size_t nlen,
                                   const char *value, size_t vlen)
{
    apr_status_t status = APR_SUCCESS;
    
    if (nlen <= 0) {
        return status;
    }
    
    if (name[0] == ':') {
        /* pseudo header, see ch. 8.1.2.3, always should come first */
        if (!apr_is_empty_table(req->headers)) {
            ap_log_perror(APLOG_MARK, APLOG_ERR, 0, pool,
                          APLOGNO(02917) 
                          "h2_request: pseudo header after request start");
            return APR_EGENERAL;
        }
        
        if (H2_HEADER_METHOD_LEN == nlen
            && !strncmp(H2_HEADER_METHOD, name, nlen)) {
            req->method = apr_pstrndup(pool, value, vlen);
        }
        else if (H2_HEADER_SCHEME_LEN == nlen
                 && !strncmp(H2_HEADER_SCHEME, name, nlen)) {
            req->scheme = apr_pstrndup(pool, value, vlen);
        }
        else if (H2_HEADER_PATH_LEN == nlen
                 && !strncmp(H2_HEADER_PATH, name, nlen)) {
            req->path = apr_pstrndup(pool, value, vlen);
        }
        else if (H2_HEADER_AUTH_LEN == nlen
                 && !strncmp(H2_HEADER_AUTH, name, nlen)) {
            req->authority = apr_pstrndup(pool, value, vlen);
        }
        else {
            char buffer[32];
            memset(buffer, 0, 32);
            strncpy(buffer, name, (nlen > 31)? 31 : nlen);
            ap_log_perror(APLOG_MARK, APLOG_WARNING, 0, pool,
                          APLOGNO(02954) 
                          "h2_request: ignoring unknown pseudo header %s",
                          buffer);
        }
    }
    else {
        /* non-pseudo header, append to work bucket of stream */
        status = h2_req_add_header(req->headers, pool, name, nlen, value, vlen);
    }
    
    return status;
}