void sieve_srs_init(void)
{
    const char *srs_domain = config_getstring(IMAPOPT_SRS_DOMAIN);
    char *saved_secrets = NULL;
    int srs_status = SRS_SUCCESS;

    if (!srs_engine && srs_domain && *srs_domain) {
        /* SRS enabled and not yet initialized */
        int srs_alwaysrewrite = config_getswitch(IMAPOPT_SRS_ALWAYSREWRITE);
        int srs_hashlength = config_getint(IMAPOPT_SRS_HASHLENGTH);
        const char *srs_separator = config_getstring(IMAPOPT_SRS_SEPARATOR);
        const char *srs_secrets = config_getstring(IMAPOPT_SRS_SECRETS);

        SRS_INIT_FAIL_UNLESS(srs_set_malloc((srs_malloc_t)xmalloc,
                                            (srs_realloc_t)xrealloc,
                                            (srs_free_t)free));

        srs_engine = srs_new();
        SRS_INIT_FAIL_UNLESS(srs_set_alwaysrewrite(srs_engine,
                                                   srs_alwaysrewrite));

        if (srs_hashlength > 0) {
            SRS_INIT_FAIL_UNLESS(srs_set_hashlength(srs_engine,
                                                    srs_hashlength));
        }
        if (srs_separator) {
            SRS_INIT_FAIL_UNLESS(srs_set_separator(srs_engine,
                                                   srs_separator[0]));
        }

        if (srs_secrets) {
            char *secret = NULL;

            saved_secrets = xstrdup(srs_secrets);
            secret = strtok(saved_secrets, ", \t\r\n");
            while (secret) {
                SRS_INIT_FAIL_UNLESS(srs_add_secret(srs_engine, secret));
                secret = strtok(NULL, ", \t\r\n");
            }
        }
    }

  END:
    if (saved_secrets) free(saved_secrets);

    if (srs_status != SRS_SUCCESS) {
        sieve_srs_free();

        syslog(LOG_ERR, "sieve SRS configuration error: %s",
               srs_strerror(srs_status));
    }
}