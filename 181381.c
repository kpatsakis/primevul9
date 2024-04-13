static void coroutine_fn v9fs_statfs(void *opaque)
{
    int32_t fid;
    ssize_t retval = 0;
    size_t offset = 7;
    V9fsFidState *fidp;
    struct statfs stbuf;
    V9fsPDU *pdu = opaque;
    V9fsState *s = pdu->s;

    retval = pdu_unmarshal(pdu, offset, "d", &fid);
    if (retval < 0) {
        goto out_nofid;
    }
    fidp = get_fid(pdu, fid);
    if (fidp == NULL) {
        retval = -ENOENT;
        goto out_nofid;
    }
    retval = v9fs_co_statfs(pdu, &fidp->path, &stbuf);
    if (retval < 0) {
        goto out;
    }
    retval = v9fs_fill_statfs(s, pdu, &stbuf);
    if (retval < 0) {
        goto out;
    }
    retval += offset;
out:
    put_fid(pdu, fidp);
out_nofid:
    pdu_complete(pdu, retval);
}