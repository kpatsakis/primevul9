static int sieve_discard(void *ac __attribute__((unused)),
                         void *ic __attribute__((unused)),
                         void *sc __attribute__((unused)),
                         void *mc,
                         const char **errmsg __attribute__((unused)))
{
    message_data_t *md = ((deliver_data_t *) mc)->m;

    prometheus_increment(CYRUS_LMTP_SIEVE_DISCARD_TOTAL);
    snmp_increment(SIEVE_DISCARD, 1);

    /* ok, we won't file it, but log it */
    syslog(LOG_INFO, "sieve discarded: %s",
           md->id ? md->id : "<nomsgid>");
    if (config_auditlog)
        syslog(LOG_NOTICE, "auditlog: discard sessionid=<%s> message-id=%s",
               session_id(), md->id ? md->id : "<nomsgid>");

    return SIEVE_OK;
}