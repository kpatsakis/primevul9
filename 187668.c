void shut_down(int code)
{
    int i;
    int bytes_in = 0;
    int bytes_out = 0;

    in_shutdown = 1;

    if (allow_cors) free_wildmats(allow_cors);

    /* Do any namespace specific cleanup */
    for (i = 0; http_namespaces[i]; i++) {
        if (http_namespaces[i]->enabled && http_namespaces[i]->shutdown)
            http_namespaces[i]->shutdown();
    }

    xmlCleanupParser();

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

    index_text_extractor_destroy();

    annotatemore_close();

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

        /* one less active connection */
        prometheus_decrement(CYRUS_HTTP_ACTIVE_CONNECTIONS);
    }
    else {
        /* one less ready listener */
        prometheus_decrement(CYRUS_HTTP_READY_LISTENERS);
    }

    prometheus_increment(code ? CYRUS_HTTP_SHUTDOWN_TOTAL_STATUS_ERROR
                              : CYRUS_HTTP_SHUTDOWN_TOTAL_STATUS_OK);

    if (protin) protgroup_free(protin);

    if (config_auditlog)
        syslog(LOG_NOTICE,
               "auditlog: traffic sessionid=<%s> bytes_in=<%d> bytes_out=<%d>",
               session_id(), bytes_in, bytes_out);

#ifdef HAVE_SSL
    tls_shutdown_serverengine();
#endif

    saslprops_free(&saslprops);

    http2_done();

    cyrus_done();

    exit(code);
}