static int send_forward(sieve_redirect_context_t *rc,
                        struct sieve_interp_ctx *ctx,
                        char *return_path,
                        struct protstream *file)
{
    int r = 0;
    char buf[1024];
    int body = 0, skip;
    char *srs_return_path = NULL;
    smtp_envelope_t sm_env = SMTP_ENVELOPE_INITIALIZER;
    struct buf msgbuf = BUF_INITIALIZER;
    smtpclient_t *sm = NULL;

    srs_return_path = sieve_srs_forward(return_path);
    if (srs_return_path) {
        smtp_envelope_set_from(&sm_env, srs_return_path);
    }
    else if (return_path && *return_path) {
        smtp_envelope_set_from(&sm_env, return_path);
    }
    else {
        smtp_envelope_set_from(&sm_env, "<>");
    }

    if (rc->is_ext_list) {
#ifdef WITH_DAV
        char *abook = get_addrbook_mboxname(rc->addr, ctx->userid);

        if (abook && !ctx->carddavdb) {
            /* open user's CardDAV DB */
            ctx->carddavdb = carddav_open_userid(ctx->userid);
        }
        if (!(abook && ctx->carddavdb)) {
            r = SIEVE_FAIL;
            free(abook);
            goto done;
        }
        carddav_foreach(ctx->carddavdb, abook, &list_addresses, &sm_env);
        free(abook);
#endif
    }
    else {
        smtp_envelope_add_rcpt(&sm_env, rc->addr);
    }

    if (srs_return_path) free(srs_return_path);

    prot_rewind(file);
    while (prot_fgets(buf, sizeof(buf), file)) {
        if (!body && buf[0] == '\r' && buf[1] == '\n') {
            /* blank line between header and body */
            body = 1;
        }

        skip = 0;
        if (!body) {
            if (!strncasecmp(buf, "Return-Path:", 12)) {
                /* strip the Return-Path */
                skip = 1;
            }
        }

        do {
            if (!skip) buf_appendcstr(&msgbuf, buf);
        } while (buf[strlen(buf)-1] != '\n' &&
                 prot_fgets(buf, sizeof(buf), file));
    }

    r = smtpclient_open(&sm);
    if (r) goto done;

    smtpclient_set_auth(sm, ctx->userid);
    smtpclient_set_notify(sm, rc->dsn_notify);
    smtpclient_set_ret(sm, rc->dsn_ret);
    smtpclient_set_by(sm, rc->deliverby);
    r = smtpclient_send(sm, &sm_env, &msgbuf);
    smtpclient_close(&sm);

done:
    smtp_envelope_fini(&sm_env);
    buf_free(&msgbuf);
    return r;
}