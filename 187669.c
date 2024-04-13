static int meth_get(struct transaction_t *txn,
                    void *params __attribute__((unused)))
{
    int ret = 0, r, fd = -1, precond, len;
    const char *prefix, *urls, *path, *ext;
    static struct buf pathbuf = BUF_INITIALIZER;
    struct stat sbuf;
    const char *msg_base = NULL;
    size_t msg_size = 0;
    struct resp_body_t *resp_body = &txn->resp_body;

    /* Upgrade to WebSockets over HTTP/1.1 on root, if requested */
    if (!strcmp(txn->req_uri->path, "/")) {
        if (txn->flags.upgrade & UPGRADE_WS) {
            return ws_start_channel(txn, NULL, &ws_echo);
        }
        else if (ws_enabled()) {
            txn->flags.upgrade |= UPGRADE_WS;
            txn->flags.conn |= CONN_UPGRADE;
        }
    }

    /* Check if this is a request for /.well-known/ listing */
    len = strlen(WELL_KNOWN_PREFIX);
    if (!strncmp(txn->req_uri->path, WELL_KNOWN_PREFIX, len)) {
        if (txn->req_uri->path[len] == '/') len++;
        if (txn->req_uri->path[len] == '\0') return list_well_known(txn);
        else return HTTP_NOT_FOUND;
    }

    /* Serve up static pages */
    prefix = config_getstring(IMAPOPT_HTTPDOCROOT);
    if (!prefix) return HTTP_NOT_FOUND;

    if (*prefix != '/') {
        /* Remote content */
        struct backend *be;

        be = proxy_findserver(prefix, &http_protocol, httpd_userid,
                              &backend_cached, NULL, NULL, httpd_in);
        if (!be) return HTTP_UNAVAILABLE;

        return http_pipe_req_resp(be, txn);
    }

    /* Local content */
    if ((urls = config_getstring(IMAPOPT_HTTPALLOWEDURLS))) {
        tok_t tok = TOK_INITIALIZER(urls, " \t", TOK_TRIMLEFT|TOK_TRIMRIGHT);
        char *token;

        while ((token = tok_next(&tok)) && strcmp(token, txn->req_uri->path));
        tok_fini(&tok);

        if (!token) return HTTP_NOT_FOUND;
    }

    buf_setcstr(&pathbuf, prefix);
    buf_appendcstr(&pathbuf, txn->req_uri->path);
    path = buf_cstring(&pathbuf);

    /* See if path is a directory and look for index.html */
    if (!(r = stat(path, &sbuf)) && S_ISDIR(sbuf.st_mode)) {
        buf_appendcstr(&pathbuf, "/index.html");
        path = buf_cstring(&pathbuf);
        r = stat(path, &sbuf);
    }

    /* See if file exists and get Content-Length & Last-Modified time */
    if (r || !S_ISREG(sbuf.st_mode)) return HTTP_NOT_FOUND;

    if (!resp_body->type) {
        /* Caller hasn't specified the Content-Type */
        resp_body->type = "application/octet-stream";

        if ((ext = strrchr(path, '.'))) {
            /* Try to use filename extension to identity Content-Type */
            const struct mimetype *mtype;

            for (mtype = mimetypes; mtype->ext; mtype++) {
                if (!strcasecmp(ext, mtype->ext)) {
                    resp_body->type = mtype->type;
                    if (!mtype->compressible) {
                        /* Never compress non-compressible resources */
                        txn->resp_body.enc.type = CE_IDENTITY;
                        txn->resp_body.enc.proc = NULL;
                        txn->flags.te = TE_NONE;
                        txn->flags.vary &= ~VARY_AE;
                    }
                    break;
                }
            }
        }
    }

    /* Generate Etag */
    assert(!buf_len(&txn->buf));
    buf_printf(&txn->buf, "%ld-%ld", (long) sbuf.st_mtime, (long) sbuf.st_size);

    /* Check any preconditions, including range request */
    txn->flags.ranges = 1;
    precond = check_precond(txn, buf_cstring(&txn->buf), sbuf.st_mtime);

    switch (precond) {
    case HTTP_OK:
    case HTTP_PARTIAL:
    case HTTP_NOT_MODIFIED:
        /* Fill in ETag, Last-Modified, and Expires */
        resp_body->etag = buf_cstring(&txn->buf);
        resp_body->lastmod = sbuf.st_mtime;
        resp_body->maxage = 86400;  /* 24 hrs */
        txn->flags.cc |= CC_MAXAGE;
        if (!httpd_userisanonymous) txn->flags.cc |= CC_PUBLIC;

        if (precond != HTTP_NOT_MODIFIED) break;

        GCC_FALLTHROUGH

    default:
        /* We failed a precondition - don't perform the request */
        resp_body->type = NULL;
        return precond;
    }

    if (txn->meth == METH_GET) {
        /* Open and mmap the file */
        if ((fd = open(path, O_RDONLY)) == -1) return HTTP_SERVER_ERROR;
        map_refresh(fd, 1, &msg_base, &msg_size, sbuf.st_size, path, NULL);
    }

    write_body(precond, txn, msg_base, sbuf.st_size);

    if (fd != -1) {
        map_free(&msg_base, &msg_size);
        close(fd);
    }

    return ret;
}