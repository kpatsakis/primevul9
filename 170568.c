static int sieve_snooze(void *ac,
                        void *ic __attribute__((unused)),
                        void *sc,
                        void *mc,
                        const char **errmsg __attribute__((unused)))
{
    sieve_snooze_context_t *sn = (sieve_snooze_context_t *) ac;
    script_data_t *sd = (script_data_t *) sc;
    deliver_data_t *mdata = (deliver_data_t *) mc;
    message_data_t *md = mdata->m;
    int quotaoverride = msg_getrcpt_ignorequota(md, mdata->cur_rcpt);
    const char *userid = mbname_userid(sd->mbname);
    int ret = IMAP_MAILBOX_NONEXISTENT;

    if (sd->edited_header) {
        mdata = setup_special_delivery(mdata);
        if (!mdata) return SIEVE_FAIL;
        else md = mdata->m;
    }

    char *intname = mboxlist_find_specialuse("\\Snoozed", userid);
    ret = mboxlist_lookup(intname, NULL, NULL);

    // didn't resolve a name, this will always fail
    if (ret) goto done;

    /* Determine until time */
    time_t now = time(NULL), until;
    struct tm *tm = localtime(&now);
    int i, day_inc = -1;
    unsigned t;
    char tbuf[26];

    if (sn->days & (1 << tm->tm_wday)) {
        /* We have times for today - see if a future one is still available */
        unsigned now_min = 60 * tm->tm_hour + tm->tm_min;

        for (i = 0; i < arrayu64_size(sn->times); i++) {
            t = arrayu64_nth(sn->times, i);
            if (t >= now_min) {
                day_inc = 0;
                break;
            }
        }
    }
    if (day_inc == -1) {
        /* Use first time on next available day */
        t = arrayu64_nth(sn->times, 0);

        /* Find next available day */
        for (i = tm->tm_wday+1; i < 14; i++) {
            if (sn->days & (1 << (i % 7))) {
                day_inc = i - tm->tm_wday;
                break;
            }
        }
    }

    tm->tm_mday += day_inc;
    tm->tm_hour = t / 60;
    tm->tm_min = t % 60;
    tm->tm_sec = 0;
    until = mktime(tm);
    time_to_iso8601(until, tbuf, sizeof(tbuf), 1);

    /* Create snoozeDetails annotation */
    json_t *snoozed = json_pack("{s:s}", "until", tbuf);

    if (sn->awaken_mbox) {
        char *awaken = NULL;
        const char *awakenid = NULL;
        mbentry_t *mbentry = NULL;

        if (sn->is_mboxid) {
            awaken = mboxlist_find_uniqueid(sn->awaken_mbox,
                                            userid, sd->authstate);
            if (awaken) awakenid = sn->awaken_mbox;
        }
        else {
            awaken = mboxname_from_external(sn->awaken_mbox, sd->ns, userid);
            ret = mboxlist_lookup(awaken, &mbentry, NULL);
            if (!ret) awakenid = mbentry->uniqueid;
        }

        if (awakenid) {
            json_object_set_new(snoozed, "moveToMailboxId",
                                json_string(awakenid));
        }

        mboxlist_entry_free(&mbentry);
        free(awaken);
    }

    if (sn->addflags || sn->removeflags) {
        json_t *set_keywords = json_object();

        if (sn->addflags) {
            add_keywords(sn->addflags, set_keywords, 1);
        }
        if (sn->removeflags) {
            add_keywords(sn->removeflags, set_keywords, 0);
        }

        json_object_set_new(snoozed, "setKeywords", set_keywords);
    }

    struct entryattlist *annots = NULL;
    const char *annot = IMAP_ANNOT_NS "snoozed";
    const char *attrib = "value.shared";
    struct buf buf = BUF_INITIALIZER;
    char *json = json_dumps(snoozed, JSON_COMPACT);

    json_decref(snoozed);
    buf_initm(&buf, json, strlen(json));
    setentryatt(&annots, annot, attrib, &buf);
    buf_free(&buf);

    /* Add \snoozed pseudo-flag */
    strarray_t *imapflags = strarray_dup(sn->imapflags);
    strarray_add(imapflags, "\\snoozed");

    struct imap4flags imap4flags = { imapflags, sd->authstate };
    ret = deliver_mailbox(md->f, mdata->content, mdata->stage, md->size,
                          &imap4flags, annots, userid, sd->authstate, md->id,
                          userid, mdata->notifyheader,
                          intname, md->date, until, quotaoverride, 0);

    strarray_free(imapflags);
    freeentryatts(annots);

done:
    if (sd->edited_header) cleanup_special_delivery(mdata);

    if (!ret) {
        prometheus_increment(CYRUS_LMTP_SIEVE_SNOOZE_TOTAL);
        snmp_increment(SIEVE_SNOOZE, 1);
        ret = SIEVE_OK;
    } else {
        *errmsg = error_message(ret);
        ret = SIEVE_FAIL;
    }

    free(intname);

    return ret;
}