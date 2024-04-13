static char *sieve_srs_forward(char *return_path)
{
    const char *srs_domain = config_getstring(IMAPOPT_SRS_DOMAIN);
    char *srs_return_path = NULL;
    int srs_status;

    if (!srs_engine) {
        /* SRS not enabled */
        return NULL;
    }

    srs_status = srs_forward_alloc(srs_engine, &srs_return_path,
                                   return_path, srs_domain);

    if (srs_status != SRS_SUCCESS) {
        syslog(LOG_ERR, "sieve SRS forward failed (%s, %s): %s",
               return_path, srs_domain, srs_strerror(srs_status));
        if (srs_return_path) {
            free(srs_return_path);
            srs_return_path = NULL;
        }
    }

    return srs_return_path;
}