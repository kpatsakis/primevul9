int service_main(int argc __attribute__((unused)),
                 char **argv __attribute__((unused)),
                 char **envp __attribute__((unused)))
{
    sasl_security_properties_t *secprops=NULL;
    const char *mechlist, *mech;
    int mechcount = 0;
    size_t mechlen;
    struct auth_scheme_t *scheme;
    struct http_connection http_conn;

    prometheus_decrement(CYRUS_HTTP_READY_LISTENERS);

    session_new_id();

    signals_poll();

    httpd_in = prot_new(0, 0);
    httpd_out = prot_new(1, 1);
    protgroup_insert(protin, httpd_in);

    /* Setup HTTP connection */
    memset(&http_conn, 0, sizeof(struct http_connection));
    http_conn.pin = httpd_in;
    http_conn.pout = httpd_out;
    http_conn.logfd = -1;

    /* Create XML parser context */
    if (!(http_conn.xml = xmlNewParserCtxt())) {
        fatal("Unable to create XML parser", EX_TEMPFAIL);
    }

    /* Find out name of client host */
    http_conn.clienthost = get_clienthost(0, &httpd_localip, &httpd_remoteip);

    if (httpd_localip && httpd_remoteip) {
        buf_setcstr(&saslprops.ipremoteport, httpd_remoteip);
        buf_setcstr(&saslprops.iplocalport, httpd_localip);
    }

    /* other params should be filled in */
    if (sasl_server_new("HTTP", config_servername, NULL,
                        buf_cstringnull_ifempty(&saslprops.iplocalport),
                        buf_cstringnull_ifempty(&saslprops.ipremoteport),
                        NULL, SASL_USAGE_FLAGS, &httpd_saslconn) != SASL_OK)
        fatal("SASL failed initializing: sasl_server_new()",EX_TEMPFAIL);

    /* will always return something valid */
    secprops = mysasl_secprops(0);

    /* no HTTP clients seem to use "auth-int" */
    secprops->max_ssf = 0;                              /* "auth" only */
    secprops->maxbufsize = 0;                           /* don't need maxbuf */
    if (sasl_setprop(httpd_saslconn, SASL_SEC_PROPS, secprops) != SASL_OK)
        fatal("Failed to set SASL property", EX_TEMPFAIL);
    if (sasl_setprop(httpd_saslconn, SASL_SSF_EXTERNAL, &extprops_ssf) != SASL_OK)
        fatal("Failed to set SASL property", EX_TEMPFAIL);

    if (httpd_remoteip) {
        char hbuf[NI_MAXHOST], *p;

        /* Create pre-authentication telemetry log based on client IP */
        strlcpy(hbuf, httpd_remoteip, NI_MAXHOST);
        if ((p = strchr(hbuf, ';'))) *p = '\0';
        http_conn.logfd = telemetry_log(hbuf, httpd_in, httpd_out, 0);
    }

    /* See which auth schemes are available to us */
    avail_auth_schemes = 0; /* Reset auth schemes for each connection */
    if ((extprops_ssf >= 2) || config_getswitch(IMAPOPT_ALLOWPLAINTEXT)) {
        avail_auth_schemes |=  AUTH_BASIC;
    }
    sasl_listmech(httpd_saslconn, NULL, NULL, " ", NULL,
                  &mechlist, NULL, &mechcount);
    for (mech = mechlist; mechcount--; mech += ++mechlen) {
        mechlen = strcspn(mech, " \0");
        for (scheme = auth_schemes; scheme->name; scheme++) {
            if (scheme->saslmech && !strncmp(mech, scheme->saslmech, mechlen)) {
                avail_auth_schemes |= scheme->id;
                break;
            }
        }
    }
    httpd_tls_required =
        config_getswitch(IMAPOPT_TLS_REQUIRED) || !avail_auth_schemes;

    proc_register(config_ident, http_conn.clienthost, NULL, NULL, NULL);

    /* Set inactivity timer */
    httpd_timeout = config_getduration(IMAPOPT_HTTPTIMEOUT, 'm');
    if (httpd_timeout < 0) httpd_timeout = 0;
    prot_settimeout(httpd_in, httpd_timeout);
    prot_setflushonread(httpd_in, httpd_out);

    /* we were connected on https port so we should do
       TLS negotiation immediately */
    if (https == 1) {
        if (starttls(NULL, &http_conn) != 0) shut_down(0);
    }
    else if (http2_preface(&http_conn)) {
        /* HTTP/2 client connection preface */
        if (http2_start_session(NULL, &http_conn) != 0)
            fatal("Failed initializing HTTP/2 session", EX_TEMPFAIL);
    }

    /* Setup the signal handler for keepalive heartbeat */
    httpd_keepalive = config_getduration(IMAPOPT_HTTPKEEPALIVE, 's');
    if (httpd_keepalive < 0) httpd_keepalive = 0;
    if (httpd_keepalive) {
        struct sigaction action;

        sigemptyset(&action.sa_mask);
        action.sa_flags = 0;
#ifdef SA_RESTART
        action.sa_flags |= SA_RESTART;
#endif
        action.sa_handler = sigalrm_handler;
        if (sigaction(SIGALRM, &action, NULL) < 0) {
            syslog(LOG_ERR, "unable to install signal handler for %d: %m", SIGALRM);
            httpd_keepalive = 0;
        }
    }

    index_text_extractor_init(httpd_in);

    prometheus_increment(CYRUS_HTTP_CONNECTIONS_TOTAL);
    prometheus_increment(CYRUS_HTTP_ACTIVE_CONNECTIONS);

    cmdloop(&http_conn);

    prometheus_decrement(CYRUS_HTTP_ACTIVE_CONNECTIONS);

    /* Closing connection */

    /* cleanup */
    signal(SIGALRM, SIG_IGN);
    httpd_reset(&http_conn);

    prometheus_increment(CYRUS_HTTP_READY_LISTENERS);

    return 0;
}