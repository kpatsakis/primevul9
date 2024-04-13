static int http1_input(struct transaction_t *txn)
{
    struct request_line_t *req_line = &txn->req_line;
    int ignore_empty = 1, ret = 0;

    do {
        /* Read request-line */
        syslog(LOG_DEBUG, "read & parse request-line");
        if (!prot_fgets(req_line->buf, MAX_REQ_LINE+1, httpd_in)) {
            txn->error.desc = prot_error(httpd_in);
            if (txn->error.desc && strcmp(txn->error.desc, PROT_EOF_STRING)) {
                /* client timed out */
                syslog(LOG_WARNING, "%s, closing connection", txn->error.desc);
                ret = HTTP_TIMEOUT;
            }
            else {
                /* client closed connection */
                syslog(LOG_DEBUG, "client closed connection");
            }

            txn->flags.conn = CONN_CLOSE;
            return ret;
        }

        /* Ignore 1 empty line before request-line per RFC 7230 Sec 3.5 */
    } while (ignore_empty-- && (strcspn(req_line->buf, "\r\n") == 0));


    /* Parse request-line = method SP request-target SP HTTP-version CRLF */
    ret = parse_request_line(txn);

    /* Parse headers */
    if (!ret) {
        ret = http_read_headers(httpd_in, 1 /* read_sep */,
                                &txn->req_hdrs, &txn->error.desc);
    }

    if (ret) {
        txn->flags.conn = CONN_CLOSE;
        goto done;
    }

    /* Examine request */
    ret = examine_request(txn, NULL);
    if (ret) goto done;

    /* Start method processing alarm (HTTP/1.1 only) */
    if (txn->flags.ver == VER_1_1) alarm(httpd_keepalive);

    /* Process the requested method */
    ret = process_request(txn);

  done:
    /* Handle errors (success responses handled by method functions) */
    if (ret) error_response(ret, txn);

    /* Read and discard any unread request body */
    if (!(txn->flags.conn & CONN_CLOSE)) {
        txn->req_body.flags |= BODY_DISCARD;
        if (http_read_req_body(txn)) {
            txn->flags.conn = CONN_CLOSE;
        }
    }

    return 0;
}