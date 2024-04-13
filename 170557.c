static int sieve_fileinto(void *ac,
                          void *ic __attribute__((unused)),
                          void *sc,
                          void *mc,
                          const char **errmsg __attribute__((unused)))
{
    sieve_fileinto_context_t *fc = (sieve_fileinto_context_t *) ac;
    script_data_t *sd = (script_data_t *) sc;
    deliver_data_t *mdata = (deliver_data_t *) mc;
    message_data_t *md = mdata->m;
    int quotaoverride = msg_getrcpt_ignorequota(md, mdata->cur_rcpt);
    struct imap4flags imap4flags = { fc->imapflags, sd->authstate };
    int ret = IMAP_MAILBOX_NONEXISTENT;

    const char *userid = mbname_userid(sd->mbname);
    char *intname;

    if (sd->edited_header) {
        mdata = setup_special_delivery(mdata);
        if (!mdata) return SIEVE_FAIL;
        else md = mdata->m;
    }

    if (fc->mailboxid) {
        intname = mboxlist_find_uniqueid(fc->mailboxid, userid, sd->authstate);
    }
    else {
        if (fc->specialuse) {
            intname = mboxname_from_external(fc->specialuse, sd->ns, userid);
            ret = mboxlist_lookup(intname, NULL, NULL);
            if (ret) free(intname);
        }
        if (ret) {
            intname = mboxname_from_externalUTF8(fc->mailbox, sd->ns, userid);
        }
    }

    // didn't resolve a name, this will always fail
    if (!intname) goto done;

    ret = deliver_mailbox(md->f, mdata->content, mdata->stage, md->size,
                          &imap4flags, NULL, userid, sd->authstate, md->id,
                          userid, mdata->notifyheader,
                          intname, md->date, 0 /*savedate*/, quotaoverride, 0);

    if (ret == IMAP_MAILBOX_NONEXISTENT) {
        /* if "plus" folder under INBOX, then try to create it */
        ret = autosieve_createfolder(userid, sd->authstate,
                                     intname, fc->do_create);

        /* Try to deliver the mail again. */
        if (!ret) {
            if (fc->specialuse) {
                /* Attempt to add special-use flag to newly created mailbox */
                struct buf specialuse = BUF_INITIALIZER;
                int r = specialuse_validate(NULL, userid, fc->specialuse, &specialuse);

                if (!r) {
                    annotatemore_write(intname, "/specialuse",
                                       userid, &specialuse);
                }
                buf_free(&specialuse);
            }

            ret = deliver_mailbox(md->f, mdata->content, mdata->stage, md->size,
                                  &imap4flags, NULL, userid, sd->authstate, md->id,
                                  userid, mdata->notifyheader,
                                  intname, md->date, 0 /*savedate*/, quotaoverride, 0);
        }
    }

done:
    if (sd->edited_header) cleanup_special_delivery(mdata);

    if (!ret) {
        prometheus_increment(CYRUS_LMTP_SIEVE_FILEINTO_TOTAL);
        snmp_increment(SIEVE_FILEINTO, 1);
        ret = SIEVE_OK;
    } else {
        *errmsg = error_message(ret);
        ret = SIEVE_FAIL;
    }

    free(intname);

    return ret;
}