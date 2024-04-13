static int coroutine_fn v9fs_xattr_fid_clunk(V9fsPDU *pdu, V9fsFidState *fidp)
{
    int retval = 0;

    if (fidp->fs.xattr.xattrwalk_fid) {
        /* getxattr/listxattr fid */
        goto free_value;
    }
    /*
     * if this is fid for setxattr. clunk should
     * result in setxattr localcall
     */
    if (fidp->fs.xattr.len != fidp->fs.xattr.copied_len) {
        /* clunk after partial write */
        retval = -EINVAL;
        goto free_out;
    }
    if (fidp->fs.xattr.len) {
        retval = v9fs_co_lsetxattr(pdu, &fidp->path, &fidp->fs.xattr.name,
                                   fidp->fs.xattr.value,
                                   fidp->fs.xattr.len,
                                   fidp->fs.xattr.flags);
    } else {
        retval = v9fs_co_lremovexattr(pdu, &fidp->path, &fidp->fs.xattr.name);
    }
free_out:
    v9fs_string_free(&fidp->fs.xattr.name);
free_value:
    g_free(fidp->fs.xattr.value);
    return retval;
}