sieve_interp_t *setup_sieve(struct sieve_interp_ctx *ctx)
{
    sieve_interp_t *interp = NULL;
    int res;
    static strarray_t mark = STRARRAY_INITIALIZER;
    static strarray_t methods = STRARRAY_INITIALIZER;

    if (!mark.count)
        strarray_append(&mark, "\\flagged");

    if (!methods.count) {
        /* XXX  is there an imapd.conf option for this? */
        strarray_append(&methods, "mailto:");
    }

    sieve_usehomedir = config_getswitch(IMAPOPT_SIEVEUSEHOMEDIR);
    if (!sieve_usehomedir) {
        sieve_dir = config_getstring(IMAPOPT_SIEVEDIR);
    } else {
        sieve_dir = NULL;
    }

    interp = sieve_interp_alloc(ctx);
    assert(interp != NULL);

    sieve_register_redirect(interp, &sieve_redirect);
    sieve_register_discard(interp, &sieve_discard);
    sieve_register_reject(interp, &sieve_reject);
    sieve_register_fileinto(interp, &sieve_fileinto);
    sieve_register_keep(interp, &sieve_keep);
    sieve_register_imapflags(interp, &mark);
    sieve_register_notify(interp, &sieve_notify, &methods);
    sieve_register_size(interp, &getsize);
    sieve_register_mailboxexists(interp, &getmailboxexists);
    sieve_register_mailboxidexists(interp, &getmailboxidexists);
    sieve_register_specialuseexists(interp, &getspecialuseexists);
    sieve_register_metadata(interp, &getmetadata);
    sieve_register_header(interp, &getheader);
    sieve_register_addheader(interp, &addheader);
    sieve_register_deleteheader(interp, &deleteheader);
    sieve_register_fname(interp, &getfname);

    sieve_register_envelope(interp, &getenvelope);
    sieve_register_environment(interp, &getenvironment);
    sieve_register_body(interp, &getbody);
    sieve_register_include(interp, &getinclude);

    sieve_register_logger(interp, &sieve_log); 

    res = sieve_register_vacation(interp, &vacation);
    if (res != SIEVE_OK) {
        syslog(LOG_ERR, "sieve_register_vacation() returns %d\n", res);
        fatal("sieve_register_vacation()", EX_SOFTWARE);
    }

    duplicate.max_expiration =
        config_getduration(IMAPOPT_SIEVE_DUPLICATE_MAX_EXPIRATION, 's');
    res = sieve_register_duplicate(interp, &duplicate);
    if (res != SIEVE_OK) {
        syslog(LOG_ERR, "sieve_register_duplicate() returns %d\n", res);
        fatal("sieve_register_duplicate()", EX_SOFTWARE);
    }

#ifdef WITH_DAV
    sieve_register_extlists(interp, &listvalidator, &listcompare);
#endif
#ifdef WITH_JMAP
    sieve_register_jmapquery(interp, &jmapquery);
#endif
#ifdef HAVE_JANSSON
    sieve_register_snooze(interp, &sieve_snooze);
#endif
    sieve_register_parse_error(interp, &sieve_parse_error_handler);
    sieve_register_execute_error(interp, &sieve_execute_error_handler);

    return interp;
}