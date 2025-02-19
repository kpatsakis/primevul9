static void coroutine_fn v9fs_symlink(void *opaque)
{
    V9fsPDU *pdu = opaque;
    V9fsString name;
    V9fsString symname;
    V9fsFidState *dfidp;
    V9fsQID qid;
    struct stat stbuf;
    int32_t dfid;
    int err = 0;
    gid_t gid;
    size_t offset = 7;

    v9fs_string_init(&name);
    v9fs_string_init(&symname);
    err = pdu_unmarshal(pdu, offset, "dssd", &dfid, &name, &symname, &gid);
    if (err < 0) {
        goto out_nofid;
    }
    trace_v9fs_symlink(pdu->tag, pdu->id, dfid, name.data, symname.data, gid);

    if (name_is_illegal(name.data)) {
        err = -ENOENT;
        goto out_nofid;
    }

    if (!strcmp(".", name.data) || !strcmp("..", name.data)) {
        err = -EEXIST;
        goto out_nofid;
    }

    dfidp = get_fid(pdu, dfid);
    if (dfidp == NULL) {
        err = -EINVAL;
        goto out_nofid;
    }
    err = v9fs_co_symlink(pdu, dfidp, &name, symname.data, gid, &stbuf);
    if (err < 0) {
        goto out;
    }
    err = stat_to_qid(pdu, &stbuf, &qid);
    if (err < 0) {
        goto out;
    }
    err =  pdu_marshal(pdu, offset, "Q", &qid);
    if (err < 0) {
        goto out;
    }
    err += offset;
    trace_v9fs_symlink_return(pdu->tag, pdu->id,
                              qid.type, qid.version, qid.path);
out:
    put_fid(pdu, dfidp);
out_nofid:
    pdu_complete(pdu, err);
    v9fs_string_free(&name);
    v9fs_string_free(&symname);
}