init_realm(kdc_realm_t *rdp, krb5_pointer aprof, char *realm, char *def_mpname,
           krb5_enctype def_enctype, char *def_udp_ports, char *def_tcp_ports,
           krb5_boolean def_manual, krb5_boolean def_restrict_anon,
           char **db_args, char *no_referral, char *hostbased)
{
    krb5_error_code     kret;
    krb5_boolean        manual;
    int                 kdb_open_flags;
    char                *svalue = NULL;
    const char          *hierarchy[4];
    krb5_kvno       mkvno = IGNORE_VNO;

    memset(rdp, 0, sizeof(kdc_realm_t));
    if (!realm) {
        kret = EINVAL;
        goto whoops;
    }
    hierarchy[0] = KRB5_CONF_REALMS;
    hierarchy[1] = realm;
    hierarchy[3] = NULL;

    rdp->realm_name = strdup(realm);
    if (rdp->realm_name == NULL) {
        kret = ENOMEM;
        goto whoops;
    }
    kret = krb5int_init_context_kdc(&rdp->realm_context);
    if (kret) {
        kdc_err(NULL, kret, _("while getting context for realm %s"), realm);
        goto whoops;
    }
    if (time_offset != 0)
        (void)krb5_set_time_offsets(rdp->realm_context, time_offset, 0);

    /* Handle master key name */
    hierarchy[2] = KRB5_CONF_MASTER_KEY_NAME;
    if (krb5_aprof_get_string(aprof, hierarchy, TRUE, &rdp->realm_mpname)) {
        rdp->realm_mpname = (def_mpname) ? strdup(def_mpname) :
            strdup(KRB5_KDB_M_NAME);
    }
    if (!rdp->realm_mpname) {
        kret = ENOMEM;
        goto whoops;
    }

    /* Handle KDC ports */
    hierarchy[2] = KRB5_CONF_KDC_PORTS;
    if (krb5_aprof_get_string(aprof, hierarchy, TRUE, &rdp->realm_ports))
        rdp->realm_ports = strdup(def_udp_ports);
    if (!rdp->realm_ports) {
        kret = ENOMEM;
        goto whoops;
    }
    hierarchy[2] = KRB5_CONF_KDC_TCP_PORTS;
    if (krb5_aprof_get_string(aprof, hierarchy, TRUE, &rdp->realm_tcp_ports))
        rdp->realm_tcp_ports = strdup(def_tcp_ports);
    if (!rdp->realm_tcp_ports) {
        kret = ENOMEM;
        goto whoops;
    }
    /* Handle stash file */
    hierarchy[2] = KRB5_CONF_KEY_STASH_FILE;
    if (krb5_aprof_get_string(aprof, hierarchy, TRUE, &rdp->realm_stash))
        manual = def_manual;
    else
        manual = FALSE;

    hierarchy[2] = KRB5_CONF_RESTRICT_ANONYMOUS_TO_TGT;
    if (krb5_aprof_get_boolean(aprof, hierarchy, TRUE,
                               &rdp->realm_restrict_anon))
        rdp->realm_restrict_anon = def_restrict_anon;

    /* Handle master key type */
    hierarchy[2] = KRB5_CONF_MASTER_KEY_TYPE;
    if (krb5_aprof_get_string(aprof, hierarchy, TRUE, &svalue) ||
        krb5_string_to_enctype(svalue, &rdp->realm_mkey.enctype))
        rdp->realm_mkey.enctype = manual ? def_enctype : ENCTYPE_UNKNOWN;
    free(svalue);
    svalue = NULL;

    /* Handle reject-bad-transit flag */
    hierarchy[2] = KRB5_CONF_REJECT_BAD_TRANSIT;
    if (krb5_aprof_get_boolean(aprof, hierarchy, TRUE,
                                &rdp->realm_reject_bad_transit))
        rdp->realm_reject_bad_transit = TRUE;

    /* Handle assume des-cbc-crc is supported for session keys */
    hierarchy[2] = KRB5_CONF_ASSUME_DES_CRC_SESSION;
    if (krb5_aprof_get_boolean(aprof, hierarchy, TRUE,
                               &rdp->realm_assume_des_crc_sess))
        rdp->realm_assume_des_crc_sess = TRUE;

    /* Handle ticket maximum life */
    hierarchy[2] = KRB5_CONF_MAX_LIFE;
    if (krb5_aprof_get_deltat(aprof, hierarchy, TRUE, &rdp->realm_maxlife))
        rdp->realm_maxlife = KRB5_KDB_MAX_LIFE;

    /* Handle ticket renewable maximum life */
    hierarchy[2] = KRB5_CONF_MAX_RENEWABLE_LIFE;
    if (krb5_aprof_get_deltat(aprof, hierarchy, TRUE, &rdp->realm_maxrlife))
        rdp->realm_maxrlife = KRB5_KDB_MAX_RLIFE;

    /* Handle KDC referrals */
    hierarchy[2] = KRB5_CONF_NO_HOST_REFERRAL;
    (void)krb5_aprof_get_string_all(aprof, hierarchy, &svalue);
    kret = combine(no_referral, svalue, &rdp->realm_no_referral);
    if (kret)
        goto whoops;
    free(svalue);
    svalue = NULL;

    hierarchy[2] = KRB5_CONF_HOST_BASED_SERVICES;
    (void)krb5_aprof_get_string_all(aprof, hierarchy, &svalue);
    kret = combine(hostbased, svalue, &rdp->realm_hostbased);
    if (kret)
        goto whoops;
    free(svalue);
    svalue = NULL;

    /*
     * We've got our parameters, now go and setup our realm context.
     */

    /* Set the default realm of this context */
    if ((kret = krb5_set_default_realm(rdp->realm_context, realm))) {
        kdc_err(rdp->realm_context, kret,
                _("while setting default realm to %s"), realm);
        goto whoops;
    }

    /* first open the database  before doing anything */
    kdb_open_flags = KRB5_KDB_OPEN_RW | KRB5_KDB_SRV_TYPE_KDC;
    if ((kret = krb5_db_open(rdp->realm_context, db_args, kdb_open_flags))) {
        kdc_err(rdp->realm_context, kret,
                _("while initializing database for realm %s"), realm);
        goto whoops;
    }

    /* Assemble and parse the master key name */
    if ((kret = krb5_db_setup_mkey_name(rdp->realm_context, rdp->realm_mpname,
                                        rdp->realm_name, (char **) NULL,
                                        &rdp->realm_mprinc))) {
        kdc_err(rdp->realm_context, kret,
                _("while setting up master key name %s for realm %s"),
                rdp->realm_mpname, realm);
        goto whoops;
    }

    /*
     * Get the master key (note, may not be the most current mkey).
     */
    if ((kret = krb5_db_fetch_mkey(rdp->realm_context, rdp->realm_mprinc,
                                   rdp->realm_mkey.enctype, manual,
                                   FALSE, rdp->realm_stash,
                                   &mkvno, NULL, &rdp->realm_mkey))) {
        kdc_err(rdp->realm_context, kret,
                _("while fetching master key %s for realm %s"),
                rdp->realm_mpname, realm);
        goto whoops;
    }

    if ((kret = krb5_db_fetch_mkey_list(rdp->realm_context, rdp->realm_mprinc,
                                        &rdp->realm_mkey))) {
        kdc_err(rdp->realm_context, kret,
                _("while fetching master keys list for realm %s"), realm);
        goto whoops;
    }


    /* Set up the keytab */
    if ((kret = krb5_ktkdb_resolve(rdp->realm_context, NULL,
                                   &rdp->realm_keytab))) {
        kdc_err(rdp->realm_context, kret,
                _("while resolving kdb keytab for realm %s"), realm);
        goto whoops;
    }

    /* Preformat the TGS name */
    if ((kret = krb5_build_principal(rdp->realm_context, &rdp->realm_tgsprinc,
                                     strlen(realm), realm, KRB5_TGS_NAME,
                                     realm, (char *) NULL))) {
        kdc_err(rdp->realm_context, kret,
                _("while building TGS name for realm %s"), realm);
        goto whoops;
    }

    if (!rkey_init_done) {
        krb5_data seed;
        /*
         * If all that worked, then initialize the random key
         * generators.
         */

        seed.length = rdp->realm_mkey.length;
        seed.data = (char *)rdp->realm_mkey.contents;

        if ((kret = krb5_c_random_add_entropy(rdp->realm_context,
                                              KRB5_C_RANDSOURCE_TRUSTEDPARTY, &seed)))
            goto whoops;

        rkey_init_done = 1;
    }
whoops:
    /*
     * If we choked, then clean up any dirt we may have dropped on the floor.
     */
    if (kret) {

        finish_realm(rdp);
    }
    return(kret);
}