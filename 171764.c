int main(int argc, char **argv)
{
    krb5_error_code     retval;
    krb5_context        kcontext;
    verto_ctx *ctx;
    int errout = 0;
    int i;

    setlocale(LC_ALL, "");
    if (strrchr(argv[0], '/'))
        argv[0] = strrchr(argv[0], '/')+1;

    shandle.kdc_realmlist = malloc(sizeof(kdc_realm_t *) *
                                   KRB5_KDC_MAX_REALMS);
    if (shandle.kdc_realmlist == NULL) {
        fprintf(stderr, _("%s: cannot get memory for realm list\n"), argv[0]);
        exit(1);
    }
    memset(shandle.kdc_realmlist, 0,
           (size_t) (sizeof(kdc_realm_t *) * KRB5_KDC_MAX_REALMS));

    /*
     * A note about Kerberos contexts: This context, "kcontext", is used
     * for the KDC operations, i.e. setup, network connection and error
     * reporting.  The per-realm operations use the "realm_context"
     * associated with each realm.
     */
    retval = krb5int_init_context_kdc(&kcontext);
    if (retval) {
        com_err(argv[0], retval, _("while initializing krb5"));
        exit(1);
    }
    krb5_klog_init(kcontext, "kdc", argv[0], 1);
    shandle.kdc_err_context = kcontext;
    kdc_progname = argv[0];
    /* N.B.: After this point, com_err sends output to the KDC log
       file, and not to stderr.  We use the kdc_err wrapper around
       com_err to ensure that the error state exists in the context
       known to the krb5_klog callback. */

    initialize_kdc5_error_table();

    /*
     * Scan through the argument list
     */
    initialize_realms(kcontext, argc, argv);

#ifndef NOCACHE
    retval = kdc_init_lookaside(kcontext);
    if (retval) {
        kdc_err(kcontext, retval, _("while initializing lookaside cache"));
        finish_realms();
        return 1;
    }
#endif

    ctx = loop_init(VERTO_EV_TYPE_NONE);
    if (!ctx) {
        kdc_err(kcontext, ENOMEM, _("while creating main loop"));
        finish_realms();
        return 1;
    }

    load_preauth_plugins(&shandle, kcontext, ctx);
    load_authdata_plugins(kcontext);

    retval = setup_sam();
    if (retval) {
        kdc_err(kcontext, retval, _("while initializing SAM"));
        finish_realms();
        return 1;
    }

    /* Handle each realm's ports */
    for (i=0; i< shandle.kdc_numrealms; i++) {
        char *cp = shandle.kdc_realmlist[i]->realm_ports;
        int port;
        while (cp && *cp) {
            if (*cp == ',' || isspace((int) *cp)) {
                cp++;
                continue;
            }
            port = strtol(cp, &cp, 10);
            if (cp == 0)
                break;
            retval = loop_add_udp_port(port);
            if (retval)
                goto net_init_error;
        }

        cp = shandle.kdc_realmlist[i]->realm_tcp_ports;
        while (cp && *cp) {
            if (*cp == ',' || isspace((int) *cp)) {
                cp++;
                continue;
            }
            port = strtol(cp, &cp, 10);
            if (cp == 0)
                break;
            retval = loop_add_tcp_port(port);
            if (retval)
                goto net_init_error;
        }
    }

    /*
     * Setup network listeners.  Disallow network reconfig in response to
     * routing socket messages if we're using worker processes, since the
     * children won't be able to re-open the listener sockets.  Hopefully our
     * platform has pktinfo support and doesn't need reconfigs.
     */
    if (workers == 0) {
        retval = loop_setup_routing_socket(ctx, &shandle, kdc_progname);
        if (retval) {
            kdc_err(kcontext, retval, _("while initializing routing socket"));
            finish_realms();
            return 1;
        }
        retval = loop_setup_signals(ctx, &shandle, reset_for_hangup);
        if (retval) {
            kdc_err(kcontext, retval, _("while initializing signal handlers"));
            finish_realms();
            return 1;
        }
    }
    if ((retval = loop_setup_network(ctx, &shandle, kdc_progname))) {
    net_init_error:
        kdc_err(kcontext, retval, _("while initializing network"));
        finish_realms();
        return 1;
    }
    if (!nofork && daemon(0, 0)) {
        kdc_err(kcontext, errno, _("while detaching from tty"));
        finish_realms();
        return 1;
    }
    if (pid_file != NULL) {
        retval = write_pid_file(pid_file);
        if (retval) {
            kdc_err(kcontext, retval, _("while creating PID file"));
            finish_realms();
            return 1;
        }
    }
    if (workers > 0) {
        finish_realms();
        retval = create_workers(ctx, workers);
        if (retval) {
            kdc_err(kcontext, errno, _("creating worker processes"));
            return 1;
        }
        /* We get here only in a worker child process; re-initialize realms. */
        initialize_realms(kcontext, argc, argv);
    }

    /* Initialize audit system and audit KDC startup. */
    retval = load_audit_modules(kcontext);
    if (retval) {
        kdc_err(kcontext, retval, _("while loading audit plugin module(s)"));
        finish_realms();
        return 1;
    }
    krb5_klog_syslog(LOG_INFO, _("commencing operation"));
    if (nofork)
        fprintf(stderr, _("%s: starting...\n"), kdc_progname);
    kau_kdc_start(kcontext, TRUE);

    verto_run(ctx);
    loop_free(ctx);
    kau_kdc_stop(kcontext, TRUE);
    krb5_klog_syslog(LOG_INFO, _("shutting down"));
    unload_preauth_plugins(kcontext);
    unload_authdata_plugins(kcontext);
    unload_audit_modules(kcontext);
    krb5_klog_close(kcontext);
    finish_realms();
    if (shandle.kdc_realmlist)
        free(shandle.kdc_realmlist);
#ifndef NOCACHE
    kdc_free_lookaside(kcontext);
#endif
    krb5_free_context(kcontext);
    return errout;
}