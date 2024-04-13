static void copy_request(char *rbuf, apr_size_t rbuflen, request_rec *r)
{
    char *p;

    if (r->the_request == NULL) {
        apr_cpystrn(rbuf, "NULL", rbuflen);
        return; /* short circuit below */
    }

    if (r->parsed_uri.password == NULL) {
        p = r->the_request;
    }
    else {
        /* Don't reveal the password in the server-status view */
        p = apr_pstrcat(r->pool, r->method, " ",
                        apr_uri_unparse(r->pool, &r->parsed_uri,
                        APR_URI_UNP_OMITPASSWORD),
                        r->assbackwards ? NULL : " ", r->protocol, NULL);
    }

    /* now figure out if we copy over the 1st rbuflen chars or the last */
    if (!ap_mod_status_reqtail) {
        apr_cpystrn(rbuf, p, rbuflen);
    }
    else {
        apr_size_t slen = strlen(p);
        if (slen < rbuflen) {
            /* it all fits anyway */
            apr_cpystrn(rbuf, p, rbuflen);
        }
        else {
            apr_cpystrn(rbuf, p+(slen-rbuflen+1), rbuflen);
        }
    }
}