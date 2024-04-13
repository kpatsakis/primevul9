static void coroutine_fn pdu_complete(V9fsPDU *pdu, ssize_t len)
{
    int8_t id = pdu->id + 1; /* Response */
    V9fsState *s = pdu->s;
    int ret;

    /*
     * The 9p spec requires that successfully cancelled pdus receive no reply.
     * Sending a reply would confuse clients because they would
     * assume that any EINTR is the actual result of the operation,
     * rather than a consequence of the cancellation. However, if
     * the operation completed (succesfully or with an error other
     * than caused be cancellation), we do send out that reply, both
     * for efficiency and to avoid confusing the rest of the state machine
     * that assumes passing a non-error here will mean a successful
     * transmission of the reply.
     */
    bool discard = pdu->cancelled && len == -EINTR;
    if (discard) {
        trace_v9fs_rcancel(pdu->tag, pdu->id);
        pdu->size = 0;
        goto out_notify;
    }

    if (len < 0) {
        int err = -len;
        len = 7;

        if (s->proto_version != V9FS_PROTO_2000L) {
            V9fsString str;

            str.data = strerror(err);
            str.size = strlen(str.data);

            ret = pdu_marshal(pdu, len, "s", &str);
            if (ret < 0) {
                goto out_notify;
            }
            len += ret;
            id = P9_RERROR;
        }

        ret = pdu_marshal(pdu, len, "d", err);
        if (ret < 0) {
            goto out_notify;
        }
        len += ret;

        if (s->proto_version == V9FS_PROTO_2000L) {
            id = P9_RLERROR;
        }
        trace_v9fs_rerror(pdu->tag, pdu->id, err); /* Trace ERROR */
    }

    /* fill out the header */
    if (pdu_marshal(pdu, 0, "dbw", (int32_t)len, id, pdu->tag) < 0) {
        goto out_notify;
    }

    /* keep these in sync */
    pdu->size = len;
    pdu->id = id;

out_notify:
    pdu->s->transport->push_and_notify(pdu);

    /* Now wakeup anybody waiting in flush for this request */
    if (!qemu_co_queue_next(&pdu->complete)) {
        pdu_free(pdu);
    }
}