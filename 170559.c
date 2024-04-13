static int sieve_reject(void *ac, void *ic,
                        void *sc, void *mc, const char **errmsg)
{
    sieve_reject_context_t *rc = (sieve_reject_context_t *) ac;
    struct sieve_interp_ctx *ctx = (struct sieve_interp_ctx *) ic;
    script_data_t *sd = (script_data_t *) sc;
    deliver_data_t *mydata = (deliver_data_t *) mc;
    message_data_t *md = mydata->m;
    const char **body;
    const char *origreceip;
    int res, need_encode = 0;

    /* Per RFC 5429, reject can do LMTP reject if reason string is US-ASCII */
    const char *cp;
    for (cp = rc->msg; *cp; cp++) {
        if (!isascii(*cp)) {
            need_encode = 1;
            break;
        }
    }

    if (rc->is_extended || (config_getswitch(IMAPOPT_SIEVE_USE_LMTP_REJECT) && !need_encode)) {
        char *msg = need_encode ?
            charset_qpencode_mimebody(rc->msg, strlen(rc->msg), 0, NULL) :
            xstrdup(rc->msg);
        strarray_t *resp = strarray_new();
        struct buf buf = BUF_INITIALIZER;
        const char *cur, *next;
        tok_t tok;

        tok_initm(&tok, msg, "\r\n", 0);
        for (cur = tok_next(&tok); (next = tok_next(&tok)); cur = next) {
            buf_printf(&buf, "550-5.7.1 %s\r\n", cur);
            strarray_appendm(resp, buf_release(&buf));
        }
        buf_printf(&buf, "550 5.7.1 %s\r\n", cur);
        strarray_appendm(resp, buf_release(&buf));
        free(msg);

        msg_setrcpt_status(md, mydata->cur_rcpt, LMTP_MESSAGE_REJECTED, resp);

        prometheus_increment(CYRUS_LMTP_SIEVE_REJECT_TOTAL);
        snmp_increment(SIEVE_REJECT, 1);
        syslog(LOG_INFO, "sieve LMTP rejected: %s",
               md->id ? md->id : "<nomsgid>");
        if (config_auditlog)
            syslog(LOG_NOTICE,
                   "auditlog: LMTP reject sessionid=<%s> message-id=%s userid=<%s>",
                   session_id(), md->id ? md->id : "<nomsgid>", ctx->userid);

        return SIEVE_OK;
    }

    if (md->return_path == NULL) {
        /* return message to who?!? */
        *errmsg = "No return-path for reply";
        return SIEVE_FAIL;
    }

    if (strlen(md->return_path) == 0) {
        syslog(LOG_INFO, "sieve: discarded reject to <> for %s id %s",
               mbname_userid(sd->mbname), md->id ? md->id : "<nomsgid>");
        if (config_auditlog)
            syslog(LOG_NOTICE,
                   "auditlog: discard-reject sessionid=<%s> message-id=%s userid=<%s>",
                   session_id(), md->id ? md->id : "<nomsgid>", ctx->userid);
        return SIEVE_OK;
    }

    body = msg_getheader(md, "original-recipient");
    origreceip = body ? body[0] : NULL;
    if ((res = send_rejection(ctx->userid, md->id, md->return_path,
                              origreceip, mbname_recipient(sd->mbname, ((deliver_data_t *) mc)->ns),
                              rc->msg, md->data)) == 0) {
        prometheus_increment(CYRUS_LMTP_SIEVE_REJECT_TOTAL);
        snmp_increment(SIEVE_REJECT, 1);
        syslog(LOG_INFO, "sieve rejected: %s to: %s",
               md->id ? md->id : "<nomsgid>", md->return_path);
        if (config_auditlog)
            syslog(LOG_NOTICE,
                   "auditlog: reject sessionid=<%s> message-id=%s target=<%s> userid=<%s>",
                   session_id(), md->id ? md->id : "<nomsgid>", md->return_path,
                   ctx->userid);
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