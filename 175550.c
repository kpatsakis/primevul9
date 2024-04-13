h2_request *h2_request_clone(apr_pool_t *p, const h2_request *src)
{
    h2_request *dst = apr_pmemdup(p, src, sizeof(*dst));
    dst->method       = apr_pstrdup(p, src->method);
    dst->scheme       = apr_pstrdup(p, src->scheme);
    dst->authority    = apr_pstrdup(p, src->authority);
    dst->path         = apr_pstrdup(p, src->path);
    dst->headers      = apr_table_clone(p, src->headers);
    return dst;
}