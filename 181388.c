static void coroutine_fn v9fs_mknod(void *opaque)
{

    int mode;
    gid_t gid;
    int32_t fid;
    V9fsQID qid;
    int err = 0;
    int major, minor;
    size_t offset = 7;
    V9fsString name;
    struct stat stbuf;
    V9fsFidState *fidp;
    V9fsPDU *pdu = opaque;

    v9fs_string_init(&name);
    err = pdu_unmarshal(pdu, offset, "dsdddd", &fid, &name, &mode,
                        &major, &minor, &gid);
    if (err < 0) {
        goto out_nofid;
    }
    trace_v9fs_mknod(pdu->tag, pdu->id, fid, mode, major, minor);

    if (name_is_illegal(name.data)) {
        err = -ENOENT;
        goto out_nofid;
    }

    if (!strcmp(".", name.data) || !strcmp("..", name.data)) {
        err = -EEXIST;
        goto out_nofid;
    }

    fidp = get_fid(pdu, fid);
    if (fidp == NULL) {
        err = -ENOENT;
        goto out_nofid;
    }
    err = v9fs_co_mknod(pdu, fidp, &name, fidp->uid, gid,
                        makedev(major, minor), mode, &stbuf);
    if (err < 0) {
        goto out;
    }
    err = stat_to_qid(pdu, &stbuf, &qid);
    if (err < 0) {
        goto out;
    }
    err = pdu_marshal(pdu, offset, "Q", &qid);
    if (err < 0) {
        goto out;
    }
    err += offset;
    trace_v9fs_mknod_return(pdu->tag, pdu->id,
                            qid.type, qid.version, qid.path);
out:
    put_fid(pdu, fidp);
out_nofid:
    pdu_complete(pdu, err);
    v9fs_string_free(&name);
}