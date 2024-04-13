static int coroutine_fn fid_to_qid(V9fsPDU *pdu, V9fsFidState *fidp,
                                   V9fsQID *qidp)
{
    struct stat stbuf;
    int err;

    err = v9fs_co_lstat(pdu, &fidp->path, &stbuf);
    if (err < 0) {
        return err;
    }
    err = stat_to_qid(pdu, &stbuf, qidp);
    if (err < 0) {
        return err;
    }
    return 0;
}