static int sieve_notify(void *ac,
                        void *interp_context __attribute__((unused)),
                        void *script_context,
                        void *mc __attribute__((unused)),
                        const char **errmsg __attribute__((unused)))
{
    const char *notifier = config_getstring(IMAPOPT_SIEVENOTIFIER);

    if (notifier) {
        sieve_notify_context_t *nc = (sieve_notify_context_t *) ac;
        script_data_t *sd = (script_data_t *) script_context;
        int nopt = 0;

        prometheus_increment(CYRUS_LMTP_SIEVE_NOTIFY_TOTAL);
        snmp_increment(SIEVE_NOTIFY, 1);

        /* count options */
        while (nc->options[nopt]) nopt++;

        /* "default" is a magic value that implies the default */
        notify(!strcmp("default",nc->method) ? notifier : nc->method,
               "SIEVE", nc->priority, mbname_userid(sd->mbname), NULL,
               nopt, nc->options, nc->message, nc->fname);
    }

    return SIEVE_OK;
}