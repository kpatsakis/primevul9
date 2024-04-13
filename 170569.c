void sieve_log(void *sc, void *mc, const char *text)
{
    script_data_t *sd = (script_data_t *) sc;
    message_data_t *md = ((deliver_data_t *) mc)->m;

    syslog(LOG_INFO, "sieve log: userid=%s messageid=%s text=%s",
           mbname_userid(sd->mbname), md->id ? md->id : "(null)", text);
}