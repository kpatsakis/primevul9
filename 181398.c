void pdu_submit(V9fsPDU *pdu, P9MsgHeader *hdr)
{
    Coroutine *co;
    CoroutineEntry *handler;
    V9fsState *s = pdu->s;

    pdu->size = le32_to_cpu(hdr->size_le);
    pdu->id = hdr->id;
    pdu->tag = le16_to_cpu(hdr->tag_le);

    if (pdu->id >= ARRAY_SIZE(pdu_co_handlers) ||
        (pdu_co_handlers[pdu->id] == NULL)) {
        handler = v9fs_op_not_supp;
    } else if (is_ro_export(&s->ctx) && !is_read_only_op(pdu)) {
        handler = v9fs_fs_ro;
    } else {
        handler = pdu_co_handlers[pdu->id];
    }

    qemu_co_queue_init(&pdu->complete);
    co = qemu_coroutine_create(handler, pdu);
    qemu_coroutine_enter(co);
}