GuestFilesystemInfoList *qmp_guest_get_fsinfo(Error **errp)
{
    FsMountList mounts;
    struct FsMount *mount;
    GuestFilesystemInfoList *new, *ret = NULL;
    Error *local_err = NULL;

    QTAILQ_INIT(&mounts);
    build_fs_mount_list(&mounts, &local_err);
    if (local_err) {
        error_propagate(errp, local_err);
        return NULL;
    }

    QTAILQ_FOREACH(mount, &mounts, next) {
        g_debug("Building guest fsinfo for '%s'", mount->dirname);

        new = g_malloc0(sizeof(*ret));
        new->value = build_guest_fsinfo(mount, &local_err);
        new->next = ret;
        ret = new;
        if (local_err) {
            error_propagate(errp, local_err);
            qapi_free_GuestFilesystemInfoList(ret);
            ret = NULL;
            break;
        }
    }

    free_fs_mount_list(&mounts);
    return ret;
}