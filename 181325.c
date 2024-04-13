static int coroutine_fn free_fid(V9fsPDU *pdu, V9fsFidState *fidp)
{
    int retval = 0;

    if (fidp->fid_type == P9_FID_FILE) {
        /* If we reclaimed the fd no need to close */
        if (fidp->fs.fd != -1) {
            retval = v9fs_co_close(pdu, &fidp->fs);
        }
    } else if (fidp->fid_type == P9_FID_DIR) {
        if (fidp->fs.dir.stream != NULL) {
            retval = v9fs_co_closedir(pdu, &fidp->fs);
        }
    } else if (fidp->fid_type == P9_FID_XATTR) {
        retval = v9fs_xattr_fid_clunk(pdu, fidp);
    }
    v9fs_path_free(&fidp->path);
    g_free(fidp);
    return retval;
}