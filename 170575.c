static int sieve_redirect(void *ac, void *ic,
                          void *sc, void *mc, const char **errmsg)
{
    sieve_redirect_context_t *rc = (sieve_redirect_context_t *) ac;
    struct sieve_interp_ctx *ctx = (struct sieve_interp_ctx *) ic;
    script_data_t *sd = (script_data_t *) sc;
    deliver_data_t *mdata = (deliver_data_t *) mc;
    message_data_t *m = mdata->m;
    char buf[8192], *sievedb = NULL;
    duplicate_key_t dkey = DUPLICATE_INITIALIZER;
    int res;

    /* if we have a msgid, we can track our redirects */
    if (m->id) {
        snprintf(buf, sizeof(buf), "%s-%s", m->id, rc->addr);
        sievedb = make_sieve_db(mbname_recipient(sd->mbname, ((deliver_data_t *) mc)->ns));

        dkey.id = buf;
        dkey.to = sievedb;
        dkey.date = ((deliver_data_t *) mc)->m->date;
        /* ok, let's see if we've redirected this message before */
        if (duplicate_check(&dkey)) {
            duplicate_log(&dkey, "redirect");
            return SIEVE_OK;
        }
    }

    if (sd->edited_header) {
        mdata = setup_special_delivery(mdata);
        if (!mdata) return SIEVE_FAIL;
        else m = mdata->m;
    }

    res = send_forward(rc, ctx, m->return_path, m->data);

    if (sd->edited_header) cleanup_special_delivery(mdata);

    if (res == 0) {
        /* mark this message as redirected */
        if (sievedb) duplicate_mark(&dkey, time(NULL), 0);

        prometheus_increment(CYRUS_LMTP_SIEVE_REDIRECT_TOTAL);
        snmp_increment(SIEVE_REDIRECT, 1);
        syslog(LOG_INFO, "sieve redirected: %s to: %s",
               m->id ? m->id : "<nomsgid>", rc->addr);
        if (config_auditlog)
            syslog(LOG_NOTICE,
                   "auditlog: redirect sessionid=<%s> message-id=%s target=<%s> userid=<%s>",
                   session_id(), m->id ? m->id : "<nomsgid>", rc->addr, ctx->userid);
        return SIEVE_OK;
    } else {
        if (res == -1) {
            *errmsg = "Could not spawn sendmail process";
        } else {
            *errmsg = error_message(res);
        }
        return SIEVE_FAIL;
    }
}