static int sieve_execute_error_handler(const char *msg,
                                       void *ic  __attribute__((unused)),
                                       void *sc, void *mc)
{
    script_data_t *sd = (script_data_t *) sc;
    message_data_t *md = ((deliver_data_t *) mc)->m;

    syslog(LOG_INFO, "sieve runtime error for %s id %s: %s",
           mbname_userid(sd->mbname), md->id ? md->id : "(null)", msg);

    return SIEVE_OK;
}