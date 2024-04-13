static void coroutine_fn v9fs_readdir(void *opaque)
{
    int32_t fid;
    V9fsFidState *fidp;
    ssize_t retval = 0;
    size_t offset = 7;
    uint64_t initial_offset;
    int32_t count;
    uint32_t max_count;
    V9fsPDU *pdu = opaque;
    V9fsState *s = pdu->s;

    retval = pdu_unmarshal(pdu, offset, "dqd", &fid,
                           &initial_offset, &max_count);
    if (retval < 0) {
        goto out_nofid;
    }
    trace_v9fs_readdir(pdu->tag, pdu->id, fid, initial_offset, max_count);

    /* Enough space for a R_readdir header: size[4] Rreaddir tag[2] count[4] */
    if (max_count > s->msize - 11) {
        max_count = s->msize - 11;
        warn_report_once(
            "9p: bad client: T_readdir with count > msize - 11"
        );
    }

    fidp = get_fid(pdu, fid);
    if (fidp == NULL) {
        retval = -EINVAL;
        goto out_nofid;
    }
    if (!fidp->fs.dir.stream) {
        retval = -EINVAL;
        goto out;
    }
    if (s->proto_version != V9FS_PROTO_2000L) {
        warn_report_once(
            "9p: bad client: T_readdir request only expected with 9P2000.L "
            "protocol version"
        );
        retval = -EOPNOTSUPP;
        goto out;
    }
    count = v9fs_do_readdir(pdu, fidp, (off_t) initial_offset, max_count);
    if (count < 0) {
        retval = count;
        goto out;
    }
    retval = pdu_marshal(pdu, offset, "d", count);
    if (retval < 0) {
        goto out;
    }
    retval += count + offset;
    trace_v9fs_readdir_return(pdu->tag, pdu->id, count, retval);
out:
    put_fid(pdu, fidp);
out_nofid:
    pdu_complete(pdu, retval);
}