static void do_fcc(script_data_t *sdata, sieve_fileinto_context_t *fcc,
                   struct buf *header, const char *msg, struct buf *footer)
{
    struct appendstate as;
    const char *userid;
    char *intname;
    int r = IMAP_MAILBOX_NONEXISTENT;

    userid = mbname_userid(sdata->mbname);

    if (fcc->specialuse) {
        intname = mboxname_from_external(fcc->specialuse, sdata->ns, userid);
        r = mboxlist_lookup(intname, NULL, NULL);
        if (r) free(intname);
    }
    if (r) intname = mboxname_from_externalUTF8(fcc->mailbox, sdata->ns, userid);

    r = mboxlist_lookup(intname, NULL, NULL);
    if (r == IMAP_MAILBOX_NONEXISTENT) {
        r = autosieve_createfolder(userid, sdata->authstate,
                                   intname, fcc->do_create);

        if (!r && fcc->specialuse) {
            /* Attempt to add special-use flag to newly created mailbox */
            struct buf specialuse = BUF_INITIALIZER;
            int r2 = specialuse_validate(NULL, userid, fcc->specialuse, &specialuse);

            if (!r2) {
                annotatemore_write(intname, "/specialuse", userid, &specialuse);
            }
            buf_free(&specialuse);
        }
    }
    if (!r) {
        r = append_setup(&as, intname, userid, sdata->authstate,
                         0, NULL, NULL, 0, EVENT_MESSAGE_APPEND);
    }
    if (!r) {
        struct stagemsg *stage;
        /* post-date by 1 sec in an effort to have
           the FCC threaded AFTER the incoming message */
        time_t internaldate = time(NULL) + 1;
        FILE *f = append_newstage(intname, internaldate,
                                  strhash(intname) /* unique msgnum for reply */,
                                  &stage);
        if (f) {
            struct body *body = NULL;

            fprintf(f, "%s%s%s",
                    buf_cstring(header), msg, buf_cstring(footer));
            fclose(f);

            r = append_fromstage(&as, &body, stage,
                                 internaldate, /* createdmodseq */ 0,
                                 fcc->imapflags, 0, /* annotations */ NULL);
            if (!r) r = append_commit(&as);

            if (body) {
                message_free_body(body);
                free(body);
            }

            append_removestage(stage);
        }
        if (r || !f) append_abort(&as);
    }

    if (r) {
        syslog(LOG_NOTICE, "sieve fcc '%s' failed: %s",
               fcc->mailbox, error_message(r));
    }

    free(intname);
}