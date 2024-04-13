static void coroutine_fn v9fs_op_not_supp(void *opaque)
{
    V9fsPDU *pdu = opaque;
    pdu_complete(pdu, -EOPNOTSUPP);
}