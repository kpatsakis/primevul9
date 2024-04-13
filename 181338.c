static void coroutine_fn v9fs_clunk(void *opaque)
{
    int err;
    int32_t fid;
    size_t offset = 7;
    V9fsFidState *fidp;
    V9fsPDU *pdu = opaque;
    V9fsState *s = pdu->s;

    err = pdu_unmarshal(pdu, offset, "d", &fid);
    if (err < 0) {
        goto out_nofid;
    }
    trace_v9fs_clunk(pdu->tag, pdu->id, fid);

    fidp = clunk_fid(s, fid);
    if (fidp == NULL) {
        err = -ENOENT;
        goto out_nofid;
    }
    /*
     * Bump the ref so that put_fid will
     * free the fid.
     */
    fidp->ref++;
    err = put_fid(pdu, fidp);
    if (!err) {
        err = offset;
    }
out_nofid:
    pdu_complete(pdu, err);
}