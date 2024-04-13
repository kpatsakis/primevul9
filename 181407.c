static void coroutine_fn v9fs_lock(void *opaque)
{
    V9fsFlock flock;
    size_t offset = 7;
    struct stat stbuf;
    V9fsFidState *fidp;
    int32_t fid, err = 0;
    V9fsPDU *pdu = opaque;

    v9fs_string_init(&flock.client_id);
    err = pdu_unmarshal(pdu, offset, "dbdqqds", &fid, &flock.type,
                        &flock.flags, &flock.start, &flock.length,
                        &flock.proc_id, &flock.client_id);
    if (err < 0) {
        goto out_nofid;
    }
    trace_v9fs_lock(pdu->tag, pdu->id, fid,
                    flock.type, flock.start, flock.length);


    /* We support only block flag now (that too ignored currently) */
    if (flock.flags & ~P9_LOCK_FLAGS_BLOCK) {
        err = -EINVAL;
        goto out_nofid;
    }
    fidp = get_fid(pdu, fid);
    if (fidp == NULL) {
        err = -ENOENT;
        goto out_nofid;
    }
    err = v9fs_co_fstat(pdu, fidp, &stbuf);
    if (err < 0) {
        goto out;
    }
    err = pdu_marshal(pdu, offset, "b", P9_LOCK_SUCCESS);
    if (err < 0) {
        goto out;
    }
    err += offset;
    trace_v9fs_lock_return(pdu->tag, pdu->id, P9_LOCK_SUCCESS);
out:
    put_fid(pdu, fidp);
out_nofid:
    pdu_complete(pdu, err);
    v9fs_string_free(&flock.client_id);
}