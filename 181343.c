static void coroutine_fn v9fs_stat(void *opaque)
{
    int32_t fid;
    V9fsStat v9stat;
    ssize_t err = 0;
    size_t offset = 7;
    struct stat stbuf;
    V9fsFidState *fidp;
    V9fsPDU *pdu = opaque;
    char *basename;

    err = pdu_unmarshal(pdu, offset, "d", &fid);
    if (err < 0) {
        goto out_nofid;
    }
    trace_v9fs_stat(pdu->tag, pdu->id, fid);

    fidp = get_fid(pdu, fid);
    if (fidp == NULL) {
        err = -ENOENT;
        goto out_nofid;
    }
    err = v9fs_co_lstat(pdu, &fidp->path, &stbuf);
    if (err < 0) {
        goto out;
    }
    basename = g_path_get_basename(fidp->path.data);
    err = stat_to_v9stat(pdu, &fidp->path, basename, &stbuf, &v9stat);
    g_free(basename);
    if (err < 0) {
        goto out;
    }
    err = pdu_marshal(pdu, offset, "wS", 0, &v9stat);
    if (err < 0) {
        v9fs_stat_free(&v9stat);
        goto out;
    }
    trace_v9fs_stat_return(pdu->tag, pdu->id, v9stat.mode,
                           v9stat.atime, v9stat.mtime, v9stat.length);
    err += offset;
    v9fs_stat_free(&v9stat);
out:
    put_fid(pdu, fidp);
out_nofid:
    pdu_complete(pdu, err);
}