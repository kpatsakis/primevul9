static int list_well_known(struct transaction_t *txn)
{
    static struct buf body = BUF_INITIALIZER;
    static time_t lastmod = 0;
    struct stat sbuf;
    int precond;

    /* stat() imapd.conf for Last-Modified and ETag */
    stat(config_filename, &sbuf);
    assert(!buf_len(&txn->buf));
    buf_printf(&txn->buf, "%ld-%ld-%ld",
               compile_time, sbuf.st_mtime, sbuf.st_size);
    sbuf.st_mtime = MAX(compile_time, sbuf.st_mtime);

    /* Check any preconditions, including range request */
    txn->flags.ranges = 1;
    precond = check_precond(txn, buf_cstring(&txn->buf), sbuf.st_mtime);

    switch (precond) {
    case HTTP_OK:
    case HTTP_NOT_MODIFIED:
        /* Fill in ETag, Last-Modified, and Expires */
        txn->resp_body.etag = buf_cstring(&txn->buf);
        txn->resp_body.lastmod = sbuf.st_mtime;
        txn->resp_body.maxage = 86400;  /* 24 hrs */
        txn->flags.cc |= CC_MAXAGE;

        if (precond != HTTP_NOT_MODIFIED) break;

        GCC_FALLTHROUGH

    default:
        /* We failed a precondition - don't perform the request */
        return precond;
    }

    if (txn->resp_body.lastmod > lastmod) {
        const char *proto = NULL, *host = NULL;
        unsigned i, level = 0;

        /* Start HTML */
        buf_reset(&body);
        buf_printf_markup(&body, level, HTML_DOCTYPE);
        buf_printf_markup(&body, level++, "<html>");
        buf_printf_markup(&body, level++, "<head>");
        buf_printf_markup(&body, level,
                          "<title>%s</title>", "Well-Known Locations");
        buf_printf_markup(&body, --level, "</head>");
        buf_printf_markup(&body, level++, "<body>");
        buf_printf_markup(&body, level,
                          "<h2>%s</h2>", "Well-Known Locations");
        buf_printf_markup(&body, level++, "<ul>");

        /* Add the list of enabled /.well-known/ URLs */
        http_proto_host(txn->req_hdrs, &proto, &host);
        for (i = 0; http_namespaces[i]; i++) {

            if (http_namespaces[i]->enabled && http_namespaces[i]->well_known) {
                buf_printf_markup(&body, level,
                                  "<li><a href=\"%s://%s%s\">%s</a></li>",
                                  proto, host, http_namespaces[i]->prefix,
                                  http_namespaces[i]->well_known);
            }
        }

        /* Finish HTML */
        buf_printf_markup(&body, --level, "</ul>");
        buf_printf_markup(&body, --level, "</body>");
        buf_printf_markup(&body, --level, "</html>");

        lastmod = txn->resp_body.lastmod;
    }

    /* Output the HTML response */
    txn->resp_body.type = "text/html; charset=utf-8";
    write_body(precond, txn, buf_cstring(&body), buf_len(&body));

    return 0;
}