int service_init(int argc __attribute__((unused)),
                 char **argv __attribute__((unused)),
                 char **envp __attribute__((unused)))
{
    int r, events, opt, i;
    int allow_trace = config_getswitch(IMAPOPT_HTTPALLOWTRACE);
    unsigned long version;
    unsigned int status, patch, fix, minor, major;

    LIBXML_TEST_VERSION

    if (geteuid() == 0) fatal("must run as the Cyrus user", EX_USAGE);
    setproctitle_init(argc, argv, envp);

    /* set signal handlers */
    signals_set_shutdown(&shut_down);
    signal(SIGPIPE, SIG_IGN);

    /* load the SASL plugins */
    global_sasl_init(1, 1, mysasl_cb);

    /* setup for sending IMAP IDLE notifications */
    idle_enabled();

    /* Set namespace */
    if ((r = mboxname_init_namespace(&httpd_namespace, 1)) != 0) {
        fatal(error_message(r), EX_CONFIG);
    }

    /* open the mboxevent system */
    events = mboxevent_init();
    apns_enabled = (events & EVENT_APPLEPUSHSERVICE_DAV);

    mboxevent_setnamespace(&httpd_namespace);

    while ((opt = getopt(argc, argv, "sp:q")) != EOF) {
        switch(opt) {
        case 's': /* https (do TLS right away) */
            https = 1;
            if (!tls_enabled()) {
                fatal("https: required OpenSSL options not present",
                      EX_CONFIG);
            }
            break;

        case 'q':
            ignorequota = 1;
            break;

        case 'p': /* external protection */
            extprops_ssf = atoi(optarg);
            break;

        default:
            usage();
        }
    }

    /* Create a protgroup for input from the client and selected backend */
    protin = protgroup_new(2);

    config_httpprettytelemetry = config_getswitch(IMAPOPT_HTTPPRETTYTELEMETRY);

    if (config_getstring(IMAPOPT_HTTPALLOWCORS)) {
        allow_cors =
            split_wildmats((char *) config_getstring(IMAPOPT_HTTPALLOWCORS),
                           NULL);
    }

    /* Construct serverinfo string */
    buf_printf(&serverinfo,
               "Cyrus-HTTP/%s Cyrus-SASL/%u.%u.%u Lib/XML%s Jansson/%s",
               CYRUS_VERSION,
               SASL_VERSION_MAJOR, SASL_VERSION_MINOR, SASL_VERSION_STEP,
               LIBXML_DOTTED_VERSION, JANSSON_VERSION);

    http2_init(&serverinfo);
    ws_init(&serverinfo);

#ifdef HAVE_SSL
    version = OPENSSL_VERSION_NUMBER;
    status  = version & 0x0f; version >>= 4;
    patch   = version & 0xff; version >>= 8;
    fix     = version & 0xff; version >>= 8;
    minor   = version & 0xff; version >>= 8;
    major   = version & 0xff;
    
    buf_printf(&serverinfo, " OpenSSL/%u.%u.%u", major, minor, fix);

    if (status == 0) buf_appendcstr(&serverinfo, "-dev");
    else if (status < 15) buf_printf(&serverinfo, "-beta%u", status);
    else if (patch) buf_putc(&serverinfo, patch + 'a' - 1);
#endif

#ifdef HAVE_ZLIB
    buf_printf(&serverinfo, " Zlib/%s", ZLIB_VERSION);
#endif
#ifdef HAVE_BROTLI
    version = BrotliEncoderVersion();
    fix     = version & 0xfff; version >>= 12;
    minor   = version & 0xfff; version >>= 12;
    major   = version & 0xfff;

    buf_printf(&serverinfo, " Brotli/%u.%u.%u", major, minor, fix);
#endif
#ifdef HAVE_ZSTD
    buf_printf(&serverinfo, " Zstd/%s", ZSTD_versionString());
#endif

    /* Initialize libical */
    ical_support_init();

    /* Do any namespace specific initialization */
    config_httpmodules = config_getbitfield(IMAPOPT_HTTPMODULES);
    for (i = 0; http_namespaces[i]; i++) {
        if (allow_trace) http_namespaces[i]->allow |= ALLOW_TRACE;
        if (http_namespaces[i]->init) http_namespaces[i]->init(&serverinfo);
    }

    compile_time = calc_compile_time(__TIME__, __DATE__);

    prometheus_increment(CYRUS_HTTP_READY_LISTENERS);

    return 0;
}