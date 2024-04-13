static int send_response(void *ac, void *ic,
                         void *sc, void *mc, const char **errmsg)
{
    char outmsgid[8192], *sievedb, *subj;
    int i, sl, ret, r;
    time_t t;
    char datestr[RFC5322_DATETIME_MAX+1];
    pid_t p;
    sieve_send_response_context_t *src = (sieve_send_response_context_t *) ac;
    message_data_t *md = ((deliver_data_t *) mc)->m;
    script_data_t *sdata = (script_data_t *) sc;
    duplicate_key_t dkey = DUPLICATE_INITIALIZER;
    struct buf header = BUF_INITIALIZER, footer = BUF_INITIALIZER;
    struct buf msgbuf = BUF_INITIALIZER;
    struct sieve_interp_ctx *ctx = (struct sieve_interp_ctx *) ic;
    smtp_envelope_t sm_env = SMTP_ENVELOPE_INITIALIZER;
    smtpclient_t *sm = NULL;

    smtp_envelope_set_from(&sm_env, "<>");
    smtp_envelope_add_rcpt(&sm_env, src->addr);

    t = time(NULL);
    p = getpid();
    snprintf(outmsgid, sizeof(outmsgid), "<cmu-sieve-%d-%d-%d@%s>",
             (int) p, (int) t, global_outgoing_count++, config_servername);

    buf_printf(&header, "Message-ID: %s\r\n", outmsgid);

    time_to_rfc5322(t, datestr, sizeof(datestr));
    buf_printf(&header, "Date: %s\r\n", datestr);

    buf_printf(&header, "X-Sieve: %s\r\n", SIEVE_VERSION);

    if (strchr(src->fromaddr, '<'))
        buf_printf(&header, "From: %s\r\n", src->fromaddr);
    else
        buf_printf(&header, "From: <%s>\r\n", src->fromaddr);

    buf_printf(&header, "To: <%s>\r\n", src->addr);
    /* check that subject is sane */
    sl = strlen(src->subj);
    for (i = 0; i < sl; i++)
        if (Uiscntrl(src->subj[i])) {
            src->subj[i] = '\0';
            break;
        }
    subj = charset_encode_mimeheader(src->subj, strlen(src->subj), 0);
    buf_printf(&header, "Subject: %s\r\n", subj);
    free(subj);
    if (md->id) buf_printf(&header, "In-Reply-To: %s\r\n", md->id);
    buf_printf(&header, "Auto-Submitted: auto-replied (vacation)\r\n");
    buf_printf(&header, "MIME-Version: 1.0\r\n");
    if (src->mime) {
        buf_printf(&header, "Content-Type: multipart/mixed;"
                "\r\n\tboundary=\"%d/%s\"\r\n", (int) p, config_servername);
        buf_printf(&header, "\r\n");
        buf_printf(&header, "This is a MIME-encapsulated message\r\n");
        buf_printf(&header, "\r\n--%d/%s\r\n", (int) p, config_servername);
    } else {
        buf_printf(&header, "Content-Type: text/plain; charset=utf-8\r\n");
        buf_printf(&header, "Content-Transfer-Encoding: 8bit\r\n");
        buf_printf(&header, "\r\n");
    }

    buf_printf(&footer, "\r\n");
    if (src->mime) {
        buf_printf(&footer, "\r\n--%d/%s--\r\n", (int) p, config_servername);
    }

    buf_append(&msgbuf, &header);
    buf_appendcstr(&msgbuf, src->msg);
    buf_append(&msgbuf, &footer);

    r = smtpclient_open(&sm);
    if (!r) {
        smtpclient_set_auth(sm, ctx->userid);
        r = smtpclient_send(sm, &sm_env, &msgbuf);
    }
    smtpclient_close(&sm);

    if (r == 0) {
        sievedb = make_sieve_db(mbname_recipient(sdata->mbname, ((deliver_data_t *) mc)->ns));

        dkey.id = outmsgid;
        dkey.to = sievedb;
        dkey.date = ((deliver_data_t *) mc)->m->date;
        duplicate_mark(&dkey, t, 0);

        if (src->fcc.mailbox) {
            do_fcc(sdata, &src->fcc, &header, src->msg, &footer);
        }

        prometheus_increment(CYRUS_LMTP_SIEVE_AUTORESPOND_SENT_TOTAL);
        snmp_increment(SIEVE_VACATION_REPLIED, 1);

        ret = SIEVE_OK;
    } else {
        *errmsg = error_message(r);
        ret = SIEVE_FAIL;
    }

    buf_free(&header);
    buf_free(&footer);
    buf_free(&msgbuf);
    smtp_envelope_fini(&sm_env);

    return ret;
}