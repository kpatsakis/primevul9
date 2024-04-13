static int send_rejection(const char *userid,
                          const char *origid,
                          const char *rejto,
                          const char *origreceip,
                          const char *mailreceip,
                          const char *reason,
                          struct protstream *file)
{
    char buf[8192], *namebuf;
    int i, r = 0;
    time_t t;
    char datestr[RFC5322_DATETIME_MAX+1];
    pid_t p;
    duplicate_key_t dkey = DUPLICATE_INITIALIZER;
    struct buf msgbuf = BUF_INITIALIZER;
    smtp_envelope_t sm_env = SMTP_ENVELOPE_INITIALIZER;
    smtpclient_t *sm = NULL;

    /* Initialize SMTP envelope */
    smtp_envelope_set_from(&sm_env, "<>");
    smtp_envelope_add_rcpt(&sm_env, rejto);

    /* Build message */
    t = time(NULL);
    p = getpid();
    snprintf(buf, sizeof(buf), "<cmu-sieve-%d-%d-%d@%s>", (int) p, (int) t,
             global_outgoing_count++, config_servername);

    namebuf = make_sieve_db(mailreceip);

    time_to_rfc5322(t, datestr, sizeof(datestr));

    dkey.id = buf;
    dkey.to = namebuf;
    dkey.date = datestr;
    duplicate_mark(&dkey, t, 0);

    buf_printf(&msgbuf, "Message-ID: %s\r\n", buf);
    buf_printf(&msgbuf, "Date: %s\r\n", datestr);

    buf_printf(&msgbuf, "X-Sieve: %s\r\n", SIEVE_VERSION);
    buf_printf(&msgbuf, "From: Mail Sieve Subsystem <%s>\r\n",
            config_getstring(IMAPOPT_POSTMASTER));
    buf_printf(&msgbuf, "To: <%s>\r\n", rejto);
    buf_printf(&msgbuf, "MIME-Version: 1.0\r\n");
    buf_printf(&msgbuf, "Content-Type: "
            "multipart/report; report-type=disposition-notification;"
            "\r\n\tboundary=\"%d/%s\"\r\n", (int) p, config_servername);
    buf_printf(&msgbuf, "Subject: Automatically rejected mail\r\n");
    buf_printf(&msgbuf, "Auto-Submitted: auto-replied (rejected)\r\n");
    buf_printf(&msgbuf, "\r\nThis is a MIME-encapsulated message\r\n\r\n");

    /* this is the human readable status report */
    buf_printf(&msgbuf, "--%d/%s\r\n", (int) p, config_servername);
    buf_printf(&msgbuf, "Content-Type: text/plain; charset=utf-8\r\n");
    buf_printf(&msgbuf, "Content-Disposition: inline\r\n");
    buf_printf(&msgbuf, "Content-Transfer-Encoding: 8bit\r\n\r\n");

    buf_printf(&msgbuf, "Your message was automatically rejected by Sieve, a mail\r\n"
            "filtering language.\r\n\r\n");
    buf_printf(&msgbuf, "The following reason was given:\r\n%s\r\n\r\n", reason);

    /* this is the MDN status report */
    buf_printf(&msgbuf, "--%d/%s\r\n"
            "Content-Type: message/disposition-notification\r\n\r\n",
            (int) p, config_servername);
    buf_printf(&msgbuf, "Reporting-UA: %s; Cyrus %s/%s\r\n",
            config_servername, CYRUS_VERSION, SIEVE_VERSION);
    if (origreceip)
        buf_printf(&msgbuf, "Original-Recipient: rfc822; %s\r\n", origreceip);
    buf_printf(&msgbuf, "Final-Recipient: rfc822; %s\r\n", mailreceip);
    if (origid)
        buf_printf(&msgbuf, "Original-Message-ID: %s\r\n", origid);
    buf_printf(&msgbuf, "Disposition: "
            "automatic-action/MDN-sent-automatically; deleted\r\n");
    buf_printf(&msgbuf, "\r\n");

    /* this is the original message */
    buf_printf(&msgbuf, "--%d/%s\r\nContent-Type: message/rfc822\r\n\r\n",
            (int) p, config_servername);
    prot_rewind(file);
    while ((i = prot_read(file, buf, sizeof(buf))) > 0) {
        buf_appendmap(&msgbuf, buf, i);
    }
    buf_printf(&msgbuf, "\r\n\r\n");
    buf_printf(&msgbuf, "--%d/%s--\r\n", (int) p, config_servername);

    /* Send the mail */
    sm = NULL;
    r = smtpclient_open(&sm);
    if (!r) {
        smtpclient_set_auth(sm, userid);
        r = smtpclient_send(sm, &sm_env, &msgbuf);
    }
    if (r) {
        syslog(LOG_ERR, "sieve: send_rejection: SMTP error: %s",
                error_message(r));
    }
    smtpclient_close(&sm);

    smtp_envelope_fini(&sm_env);
    buf_free(&msgbuf);
    return r;
}