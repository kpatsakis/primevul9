V9fsPDU *pdu_alloc(V9fsState *s)
{
    V9fsPDU *pdu = NULL;

    if (!QLIST_EMPTY(&s->free_list)) {
        pdu = QLIST_FIRST(&s->free_list);
        QLIST_REMOVE(pdu, next);
        QLIST_INSERT_HEAD(&s->active_list, pdu, next);
    }
    return pdu;
}