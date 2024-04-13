static int sieve_parse_error_handler(int lineno, const char *msg,
                                     void *ic __attribute__((unused)),
                                     void *sc)
{
    script_data_t *sd = (script_data_t *) sc;

    syslog(LOG_INFO, "sieve parse error for %s: line %d: %s",
           mbname_userid(sd->mbname), lineno, msg);

    return SIEVE_OK;
}