static int set_h1_header(void *ctx, const char *key, const char *value)
{
    h1_ctx *x = ctx;
    x->status = h2_req_add_header(x->headers, x->pool, key, strlen(key), 
                                  value, strlen(value));
    return (x->status == APR_SUCCESS)? 1 : 0;
}