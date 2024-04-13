static void coroutine_fn v9fs_getlock(void *opaque)
{
    size_t offset = 7;
    struct stat stbuf;
    V9fsFidState *fidp;
    V9fsGetlock glock;
    int32_t fid, err = 0;
    V9fsPDU *pdu = opaque;

    v9fs_string_init(&glock.client_id);
    err = pdu_unmarshal(pdu, offset, "dbqqds", &fid, &glock.type,
                        &glock.start, &glock.length, &glock.proc_id,
                        &glock.client_id);
    if (err < 0) {
        goto out_nofid;
    }
    trace_v9fs_getlock(pdu->tag, pdu->id, fid,
                       glock.type, glock.start, glock.length);

    fidp = get_fid(pdu, fid);
    if (fidp == NULL) {
        err = -ENOENT;
        goto out_nofid;
    }
    err = v9fs_co_fstat(pdu, fidp, &stbuf);
    if (err < 0) {
        goto out;
    }
    glock.type = P9_LOCK_TYPE_UNLCK;
    err = pdu_marshal(pdu, offset, "bqqds", glock.type,
                          glock.start, glock.length, glock.proc_id,
                          &glock.client_id);
    if (err < 0) {
        goto out;
    }
    err += offset;
    trace_v9fs_getlock_return(pdu->tag, pdu->id, glock.type, glock.start,
                              glock.length, glock.proc_id);
out:
    put_fid(pdu, fidp);
out_nofid:
    pdu_complete(pdu, err);
    v9fs_string_free(&glock.client_id);
}