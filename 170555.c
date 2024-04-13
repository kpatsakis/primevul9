static int getinclude(void *sc, const char *script, int isglobal,
                      char *fname, size_t size)
{
    script_data_t *sdata = (script_data_t *) sc;
    struct stat sbuf;
    int r;

    if (strstr(script, "../")) {
        syslog(LOG_NOTICE, "Illegal script name '%s' for user '%s'",
               script, mbname_userid(sdata->mbname));
        return SIEVE_FAIL;
    }

    r = sieve_find_script(isglobal ? NULL : mbname_localpart(sdata->mbname),
                          mbname_domain(sdata->mbname), script, fname, size);

    if (!r && isglobal && mbname_domain(sdata->mbname) && stat(fname, &sbuf) != 0) {
        /* if the domain-specific global script doesn't exist,
           try a server-wide global script */
        r = sieve_find_script(NULL, NULL, script, fname, size);
    }

    return r;
}