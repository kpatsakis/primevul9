static void httpd_reset(struct http_connection *conn)
{
    int i;
    int bytes_in = 0;
    int bytes_out = 0;

    /* Do any namespace specific cleanup */
    for (i = 0; http_namespaces[i]; i++) {
        if (http_namespaces[i]->enabled && http_namespaces[i]->reset)
            http_namespaces[i]->reset();
    }

    /* Reset available authentication schemes */
    avail_auth_schemes = 0;

    proc_cleanup();

    /* close backend connections */
    i = 0;
    while (backend_cached && backend_cached[i]) {
        proxy_downserver(backend_cached[i]);
        free(backend_cached[i]->context);
        free(backend_cached[i]);
        i++;
    }
    if (backend_cached) free(backend_cached);
    backend_cached = NULL;
    backend_current = NULL;

    index_text_extractor_destroy();

    if (httpd_in) {
        prot_NONBLOCK(httpd_in);
        prot_fill(httpd_in);
        bytes_in = prot_bytes_in(httpd_in);
        prot_free(httpd_in);
    }

    if (httpd_out) {
        prot_flush(httpd_out);
        bytes_out = prot_bytes_out(httpd_out);
        prot_free(httpd_out);
    }

    if (config_auditlog) {
        syslog(LOG_NOTICE,
               "auditlog: traffic sessionid=<%s> bytes_in=<%d> bytes_out=<%d>",
               session_id(), bytes_in, bytes_out);
    }

    httpd_in = httpd_out = NULL;

    if (protin) protgroup_reset(protin);

#ifdef HAVE_SSL
    if (conn->tls_ctx) {
        tls_reset_servertls((SSL **) &conn->tls_ctx);
        conn->tls_ctx = NULL;
    }
#endif

    xmlFreeParserCtxt(conn->xml);

    http2_end_session(conn->sess_ctx);

    cyrus_reset_stdio();

    conn->clienthost = "[local]";
    buf_free(&conn->logbuf);
    if (conn->logfd != -1) {
        close(conn->logfd);
        conn->logfd = -1;
    }
    if (httpd_authid != NULL) {
        free(httpd_authid);
        httpd_authid = NULL;
    }
    if (httpd_userid != NULL) {
        free(httpd_userid);
        httpd_userid = NULL;
    }
    httpd_userisanonymous = 1;
    if (httpd_extrafolder != NULL) {
        free(httpd_extrafolder);
        httpd_extrafolder = NULL;
    }
    if (httpd_extradomain != NULL) {
        free(httpd_extradomain);
        httpd_extradomain = NULL;
    }
    if (httpd_authstate) {
        auth_freestate(httpd_authstate);
        httpd_authstate = NULL;
    }
    if (httpd_saslconn) {
        sasl_dispose(&httpd_saslconn);
        httpd_saslconn = NULL;
    }

    saslprops_reset(&saslprops);

    session_new_id();
}