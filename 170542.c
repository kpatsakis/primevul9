static deliver_data_t *setup_special_delivery(deliver_data_t *mydata)
{
    static deliver_data_t dd;
    static message_data_t md;
    static struct message_content mc;

    memcpy(&dd, mydata, sizeof(deliver_data_t));
    dd.m = memcpy(&md, mydata->m, sizeof(message_data_t));
    dd.content = &mc;
    memset(&mc, 0, sizeof(struct message_content));

    /* build the mailboxname from the recipient address */
    const mbname_t *origmbname = msg_getrcpt(mydata->m, mydata->cur_rcpt);

    /* do the userid */
    mbname_t *mbname = mbname_dup(origmbname);
    if (mbname_userid(mbname)) {
        mbname_truncate_boxes(mbname, 0);
    }

    const char *intname = mbname_intname(mbname);
    md.f = append_newstage(intname, time(0),
                           strhash(intname) /* unique msgnum for modified msg */,
                           &dd.stage);
    if (md.f) {
        char buf[4096];

        /* write updated message headers */
        spool_enum_hdrcache(mydata->m->hdrcache, &dump_header, md.f);

        /* get offset of message body */
        md.body_offset = ftell(md.f);

        /* write message body */
        fseek(mydata->m->f, mydata->m->body_offset, SEEK_SET);
        while (fgets(buf, sizeof(buf), mydata->m->f)) fputs(buf, md.f);
        fflush(md.f);

        /* XXX  do we look for updated Date and Message-ID? */
        md.size = ftell(md.f);
        md.data = prot_new(fileno(md.f), 0);

        mydata = &dd;
    }
    else mydata = NULL;

    mbname_free(&mbname);

    return mydata;
}