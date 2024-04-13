static int coroutine_fn v9fs_complete_rename(V9fsPDU *pdu, V9fsFidState *fidp,
                                             int32_t newdirfid,
                                             V9fsString *name)
{
    int err = 0;
    V9fsPath new_path;
    V9fsFidState *tfidp;
    V9fsState *s = pdu->s;
    V9fsFidState *dirfidp = NULL;

    v9fs_path_init(&new_path);
    if (newdirfid != -1) {
        dirfidp = get_fid(pdu, newdirfid);
        if (dirfidp == NULL) {
            return -ENOENT;
        }
        if (fidp->fid_type != P9_FID_NONE) {
            err = -EINVAL;
            goto out;
        }
        err = v9fs_co_name_to_path(pdu, &dirfidp->path, name->data, &new_path);
        if (err < 0) {
            goto out;
        }
    } else {
        char *dir_name = g_path_get_dirname(fidp->path.data);
        V9fsPath dir_path;

        v9fs_path_init(&dir_path);
        v9fs_path_sprintf(&dir_path, "%s", dir_name);
        g_free(dir_name);

        err = v9fs_co_name_to_path(pdu, &dir_path, name->data, &new_path);
        v9fs_path_free(&dir_path);
        if (err < 0) {
            goto out;
        }
    }
    err = v9fs_co_rename(pdu, &fidp->path, &new_path);
    if (err < 0) {
        goto out;
    }
    /*
     * Fixup fid's pointing to the old name to
     * start pointing to the new name
     */
    for (tfidp = s->fid_list; tfidp; tfidp = tfidp->next) {
        if (v9fs_path_is_ancestor(&fidp->path, &tfidp->path)) {
            /* replace the name */
            v9fs_fix_path(&tfidp->path, &new_path, strlen(fidp->path.data));
        }
    }
out:
    if (dirfidp) {
        put_fid(pdu, dirfidp);
    }
    v9fs_path_free(&new_path);
    return err;
}