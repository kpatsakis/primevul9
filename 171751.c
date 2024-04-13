initialize_realms(krb5_context kcontext, int argc, char **argv)
{
    int                 c;
    char                *db_name = (char *) NULL;
    char                *lrealm = (char *) NULL;
    char                *mkey_name = (char *) NULL;
    krb5_error_code     retval;
    krb5_enctype        menctype = ENCTYPE_UNKNOWN;
    kdc_realm_t         *rdatap = NULL;
    krb5_boolean        manual = FALSE;
    krb5_boolean        def_restrict_anon;
    char                *default_udp_ports = 0;
    char                *default_tcp_ports = 0;
    krb5_pointer        aprof = NULL;
    const char          *hierarchy[3];
    char                *no_referral = NULL;
    char                *hostbased = NULL;
    int                  db_args_size = 0;
    char                **db_args = NULL;

    extern char *optarg;

    if (!krb5_aprof_init(DEFAULT_KDC_PROFILE, KDC_PROFILE_ENV, &aprof)) {
        hierarchy[0] = KRB5_CONF_KDCDEFAULTS;
        hierarchy[1] = KRB5_CONF_KDC_PORTS;
        hierarchy[2] = (char *) NULL;
        if (krb5_aprof_get_string(aprof, hierarchy, TRUE, &default_udp_ports))
            default_udp_ports = 0;
        hierarchy[1] = KRB5_CONF_KDC_TCP_PORTS;
        if (krb5_aprof_get_string(aprof, hierarchy, TRUE, &default_tcp_ports))
            default_tcp_ports = 0;
        hierarchy[1] = KRB5_CONF_MAX_DGRAM_REPLY_SIZE;
        if (krb5_aprof_get_int32(aprof, hierarchy, TRUE, &max_dgram_reply_size))
            max_dgram_reply_size = MAX_DGRAM_SIZE;
        hierarchy[1] = KRB5_CONF_RESTRICT_ANONYMOUS_TO_TGT;
        if (krb5_aprof_get_boolean(aprof, hierarchy, TRUE, &def_restrict_anon))
            def_restrict_anon = FALSE;
        hierarchy[1] = KRB5_CONF_NO_HOST_REFERRAL;
        if (krb5_aprof_get_string_all(aprof, hierarchy, &no_referral))
            no_referral = 0;
        hierarchy[1] = KRB5_CONF_HOST_BASED_SERVICES;
        if (krb5_aprof_get_string_all(aprof, hierarchy, &hostbased))
            hostbased = 0;
    }

    if (default_udp_ports == 0) {
        default_udp_ports = strdup(DEFAULT_KDC_UDP_PORTLIST);
        if (default_udp_ports == 0) {
            fprintf(stderr, _(" KDC cannot initialize. Not enough memory\n"));
            exit(1);
        }
    }
    if (default_tcp_ports == 0) {
        default_tcp_ports = strdup(DEFAULT_KDC_TCP_PORTLIST);
        if (default_tcp_ports == 0) {
            fprintf(stderr, _(" KDC cannot initialize. Not enough memory\n"));
            exit(1);
        }
    }

    /*
     * Loop through the option list.  Each time we encounter a realm name,
     * use the previously scanned options to fill in for defaults.
     */
    while ((c = getopt(argc, argv, "x:r:d:mM:k:R:e:P:p:s:nw:4:T:X3")) != -1) {
        switch(c) {
        case 'x':
            db_args_size++;
            {
                char **temp = realloc( db_args, sizeof(char*) * (db_args_size+1)); /* one for NULL */
                if( temp == NULL )
                {
                    fprintf(stderr, _("%s: KDC cannot initialize. Not enough "
                                      "memory\n"), argv[0]);
                    exit(1);
                }

                db_args = temp;
            }
            db_args[db_args_size-1] = optarg;
            db_args[db_args_size]   = NULL;
            break;

        case 'r':                       /* realm name for db */
            if (!find_realm_data(&shandle, optarg, (krb5_ui_4) strlen(optarg))) {
                if ((rdatap = (kdc_realm_t *) malloc(sizeof(kdc_realm_t)))) {
                    retval = init_realm(rdatap, aprof, optarg, mkey_name,
                                        menctype, default_udp_ports,
                                        default_tcp_ports, manual,
                                        def_restrict_anon, db_args,
                                        no_referral, hostbased);
                    if (retval) {
                        fprintf(stderr, _("%s: cannot initialize realm %s - "
                                          "see log file for details\n"),
                                argv[0], optarg);
                        exit(1);
                    }
                    shandle.kdc_realmlist[shandle.kdc_numrealms] = rdatap;
                    shandle.kdc_numrealms++;
                    free(db_args), db_args=NULL, db_args_size = 0;
                }
                else
                {
                    fprintf(stderr, _("%s: cannot initialize realm %s. Not "
                                      "enough memory\n"), argv[0], optarg);
                    exit(1);
                }
            }
            break;
        case 'd':                       /* pathname for db */
            /* now db_name is not a seperate argument.
             * It has to be passed as part of the db_args
             */
            if( db_name == NULL ) {
                if (asprintf(&db_name, "dbname=%s", optarg) < 0) {
                    fprintf(stderr, _("%s: KDC cannot initialize. Not enough "
                                      "memory\n"), argv[0]);
                    exit(1);
                }
            }

            db_args_size++;
            {
                char **temp = realloc( db_args, sizeof(char*) * (db_args_size+1)); /* one for NULL */
                if( temp == NULL )
                {
                    fprintf(stderr, _("%s: KDC cannot initialize. Not enough "
                                      "memory\n"), argv[0]);
                    exit(1);
                }

                db_args = temp;
            }
            db_args[db_args_size-1] = db_name;
            db_args[db_args_size]   = NULL;
            break;
        case 'm':                       /* manual type-in of master key */
            manual = TRUE;
            if (menctype == ENCTYPE_UNKNOWN)
                menctype = ENCTYPE_DES_CBC_CRC;
            break;
        case 'M':                       /* master key name in DB */
            mkey_name = optarg;
            break;
        case 'n':
            nofork++;                   /* don't detach from terminal */
            break;
        case 'w':                       /* create multiple worker processes */
            workers = atoi(optarg);
            if (workers <= 0)
                usage(argv[0]);
            break;
        case 'k':                       /* enctype for master key */
            if (krb5_string_to_enctype(optarg, &menctype))
                com_err(argv[0], 0, _("invalid enctype %s"), optarg);
            break;
        case 'R':
            /* Replay cache name; defunct since we don't use a replay cache. */
            break;
        case 'P':
            pid_file = optarg;
            break;
        case 'p':
            if (default_udp_ports)
                free(default_udp_ports);
            default_udp_ports = strdup(optarg);
            if (!default_udp_ports) {
                fprintf(stderr, _(" KDC cannot initialize. Not enough "
                                  "memory\n"));
                exit(1);
            }
#if 0 /* not yet */
            if (default_tcp_ports)
                free(default_tcp_ports);
            default_tcp_ports = strdup(optarg);
#endif
            break;
        case 'T':
            time_offset = atoi(optarg);
            break;
        case '4':
            break;
        case 'X':
            break;
        case '?':
        default:
            usage(argv[0]);
        }
    }

    /*
     * Check to see if we processed any realms.
     */
    if (shandle.kdc_numrealms == 0) {
        /* no realm specified, use default realm */
        if ((retval = krb5_get_default_realm(kcontext, &lrealm))) {
            com_err(argv[0], retval,
                    _("while attempting to retrieve default realm"));
            fprintf (stderr,
                     _("%s: %s, attempting to retrieve default realm\n"),
                     argv[0], krb5_get_error_message(kcontext, retval));
            exit(1);
        }
        if ((rdatap = (kdc_realm_t *) malloc(sizeof(kdc_realm_t)))) {
            retval = init_realm(rdatap, aprof, lrealm, mkey_name, menctype,
                                default_udp_ports, default_tcp_ports, manual,
                                def_restrict_anon, db_args, no_referral,
                                hostbased);
            if (retval) {
                fprintf(stderr, _("%s: cannot initialize realm %s - see log "
                                  "file for details\n"), argv[0], lrealm);
                exit(1);
            }
            shandle.kdc_realmlist[0] = rdatap;
            shandle.kdc_numrealms++;
        }
        krb5_free_default_realm(kcontext, lrealm);
    }

    if (default_udp_ports)
        free(default_udp_ports);
    if (default_tcp_ports)
        free(default_tcp_ports);
    if (db_args)
        free(db_args);
    if (db_name)
        free(db_name);
    if (hostbased)
        free(hostbased);
    if (no_referral)
        free(no_referral);
    if (aprof)
        krb5_aprof_finish(aprof);

    return;
}