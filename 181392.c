static void coroutine_fn v9fs_fs_ro(void *opaque)
{
    V9fsPDU *pdu = opaque;
    pdu_complete(pdu, -EROFS);
}