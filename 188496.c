int64_t qmp_guest_fsfreeze_thaw(Error **errp)
{
    int ret;
    FsMountList mounts;
    FsMount *mount;
    int fd, i = 0, logged;
    Error *local_err = NULL;

    QTAILQ_INIT(&mounts);
    build_fs_mount_list(&mounts, &local_err);
    if (local_err) {
        error_propagate(errp, local_err);
        return 0;
    }

    QTAILQ_FOREACH(mount, &mounts, next) {
        logged = false;
        fd = qemu_open(mount->dirname, O_RDONLY);
        if (fd == -1) {
            continue;
        }
        /* we have no way of knowing whether a filesystem was actually unfrozen
         * as a result of a successful call to FITHAW, only that if an error
         * was returned the filesystem was *not* unfrozen by that particular
         * call.
         *
         * since multiple preceding FIFREEZEs require multiple calls to FITHAW
         * to unfreeze, continuing issuing FITHAW until an error is returned,
         * in which case either the filesystem is in an unfreezable state, or,
         * more likely, it was thawed previously (and remains so afterward).
         *
         * also, since the most recent successful call is the one that did
         * the actual unfreeze, we can use this to provide an accurate count
         * of the number of filesystems unfrozen by guest-fsfreeze-thaw, which
         * may * be useful for determining whether a filesystem was unfrozen
         * during the freeze/thaw phase by a process other than qemu-ga.
         */
        do {
            ret = ioctl(fd, FITHAW);
            if (ret == 0 && !logged) {
                i++;
                logged = true;
            }
        } while (ret == 0);
        close(fd);
    }

    ga_unset_frozen(ga_state);
    free_fs_mount_list(&mounts);

    execute_fsfreeze_hook(FSFREEZE_HOOK_THAW, errp);

    return i;
}