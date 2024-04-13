static void coroutine_fn v9fs_unlinkat(void *opaque)
{
    int err = 0;
    V9fsString name;
    int32_t dfid, flags, rflags = 0;
    size_t offset = 7;
    V9fsPath path;
    V9fsFidState *dfidp;
    V9fsPDU *pdu = opaque;

    v9fs_string_init(&name);
    err = pdu_unmarshal(pdu, offset, "dsd", &dfid, &name, &flags);
    if (err < 0) {
        goto out_nofid;
    }

    if (name_is_illegal(name.data)) {
        err = -ENOENT;
        goto out_nofid;
    }

    if (!strcmp(".", name.data)) {
        err = -EINVAL;
        goto out_nofid;
    }

    if (!strcmp("..", name.data)) {
        err = -ENOTEMPTY;
        goto out_nofid;
    }

    if (flags & ~P9_DOTL_AT_REMOVEDIR) {
        err = -EINVAL;
        goto out_nofid;
    }

    if (flags & P9_DOTL_AT_REMOVEDIR) {
        rflags |= AT_REMOVEDIR;
    }

    dfidp = get_fid(pdu, dfid);
    if (dfidp == NULL) {
        err = -EINVAL;
        goto out_nofid;
    }
    /*
     * IF the file is unlinked, we cannot reopen
     * the file later. So don't reclaim fd
     */
    v9fs_path_init(&path);
    err = v9fs_co_name_to_path(pdu, &dfidp->path, name.data, &path);
    if (err < 0) {
        goto out_err;
    }
    err = v9fs_mark_fids_unreclaim(pdu, &path);
    if (err < 0) {
        goto out_err;
    }
    err = v9fs_co_unlinkat(pdu, &dfidp->path, &name, rflags);
    if (!err) {
        err = offset;
    }
out_err:
    put_fid(pdu, dfidp);
    v9fs_path_free(&path);
out_nofid:
    pdu_complete(pdu, err);
    v9fs_string_free(&name);
}