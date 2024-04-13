static void coroutine_fn v9fs_xattrcreate(void *opaque)
{
    int flags, rflags = 0;
    int32_t fid;
    uint64_t size;
    ssize_t err = 0;
    V9fsString name;
    size_t offset = 7;
    V9fsFidState *file_fidp;
    V9fsFidState *xattr_fidp;
    V9fsPDU *pdu = opaque;

    v9fs_string_init(&name);
    err = pdu_unmarshal(pdu, offset, "dsqd", &fid, &name, &size, &flags);
    if (err < 0) {
        goto out_nofid;
    }
    trace_v9fs_xattrcreate(pdu->tag, pdu->id, fid, name.data, size, flags);

    if (flags & ~(P9_XATTR_CREATE | P9_XATTR_REPLACE)) {
        err = -EINVAL;
        goto out_nofid;
    }

    if (flags & P9_XATTR_CREATE) {
        rflags |= XATTR_CREATE;
    }

    if (flags & P9_XATTR_REPLACE) {
        rflags |= XATTR_REPLACE;
    }

    if (size > XATTR_SIZE_MAX) {
        err = -E2BIG;
        goto out_nofid;
    }

    file_fidp = get_fid(pdu, fid);
    if (file_fidp == NULL) {
        err = -EINVAL;
        goto out_nofid;
    }
    if (file_fidp->fid_type != P9_FID_NONE) {
        err = -EINVAL;
        goto out_put_fid;
    }

    /* Make the file fid point to xattr */
    xattr_fidp = file_fidp;
    xattr_fidp->fid_type = P9_FID_XATTR;
    xattr_fidp->fs.xattr.copied_len = 0;
    xattr_fidp->fs.xattr.xattrwalk_fid = false;
    xattr_fidp->fs.xattr.len = size;
    xattr_fidp->fs.xattr.flags = rflags;
    v9fs_string_init(&xattr_fidp->fs.xattr.name);
    v9fs_string_copy(&xattr_fidp->fs.xattr.name, &name);
    xattr_fidp->fs.xattr.value = g_malloc0(size);
    err = offset;
out_put_fid:
    put_fid(pdu, file_fidp);
out_nofid:
    pdu_complete(pdu, err);
    v9fs_string_free(&name);
}