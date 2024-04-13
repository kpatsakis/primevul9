static int auth_success(struct transaction_t *txn, const char *userid)
{
    struct auth_scheme_t *scheme = txn->auth_chal.scheme;
    int logfd = txn->conn->logfd;
    int i;

    httpd_userid = xstrdup(userid);
    httpd_userisanonymous = is_userid_anonymous(httpd_userid);

    syslog(LOG_NOTICE, "login: %s %s %s%s %s SESSIONID=<%s>",
           txn->conn->clienthost, httpd_userid, scheme->name,
           txn->conn->tls_ctx ? "+TLS" : "", "User logged in",
           session_id());


    /* Recreate telemetry log entry for request (w/ credentials redacted) */
    assert(!buf_len(&txn->buf));
    buf_printf(&txn->buf, "<%ld<", time(NULL));         /* timestamp */
    buf_printf(&txn->buf, "%s %s %s\r\n",               /* request-line*/
               txn->req_line.meth, txn->req_line.uri, txn->req_line.ver);
    spool_enum_hdrcache(txn->req_hdrs,                  /* header fields */
                        &log_cachehdr, &txn->buf);
    buf_appendcstr(&txn->buf, "\r\n");                  /* CRLF */
    buf_append(&txn->buf, &txn->req_body.payload);      /* message body */
    buf_appendmap(&txn->buf,                            /* buffered input */
                  (const char *) httpd_in->ptr, httpd_in->cnt);

    if (logfd != -1) {
        /* Rewind log to current request and truncate it */
        off_t end = lseek(logfd, 0, SEEK_END);

        if (ftruncate(logfd, end - buf_len(&txn->buf)))
            syslog(LOG_ERR, "IOERROR: failed to truncate http log");

        /* Close existing telemetry log */
        close(logfd);
    }

    prot_setlog(httpd_in, PROT_NO_FD);
    prot_setlog(httpd_out, PROT_NO_FD);

    /* Create telemetry log based on new userid */
    if (txn->conn->sess_ctx)
        txn->conn->logfd = logfd = telemetry_log(userid, NULL, NULL, 0);
    else
        txn->conn->logfd = logfd = telemetry_log(userid, httpd_in, httpd_out, 0);

    if (logfd != -1) {
        /* Log credential-redacted request */
        if (write(logfd, buf_cstring(&txn->buf), buf_len(&txn->buf)) < 0)
            syslog(LOG_ERR, "IOERROR: failed to write to http log");
    }

    buf_reset(&txn->buf);

    /* Do any namespace specific post-auth processing */
    for (i = 0; http_namespaces[i]; i++) {
        if (http_namespaces[i]->enabled && http_namespaces[i]->auth) {
            int ret = http_namespaces[i]->auth(httpd_userid);
            if (ret) return ret;
        }
    }

    return 0;
}