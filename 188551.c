int64_t qmp_guest_fsfreeze_freeze_list(bool has_mountpoints,
                                       strList *mountpoints,
                                       Error **errp)
{
    int ret = 0, i = 0;
    strList *list;
    FsMountList mounts;
    struct FsMount *mount;
    Error *local_err = NULL;
    int fd;

    slog("guest-fsfreeze called");

    execute_fsfreeze_hook(FSFREEZE_HOOK_FREEZE, &local_err);
    if (local_err) {
        error_propagate(errp, local_err);
        return -1;
    }

    QTAILQ_INIT(&mounts);
    build_fs_mount_list(&mounts, &local_err);
    if (local_err) {
        error_propagate(errp, local_err);
        return -1;
    }

    /* cannot risk guest agent blocking itself on a write in this state */
    ga_set_frozen(ga_state);

    QTAILQ_FOREACH_REVERSE(mount, &mounts, FsMountList, next) {
        /* To issue fsfreeze in the reverse order of mounts, check if the
         * mount is listed in the list here */
        if (has_mountpoints) {
            for (list = mountpoints; list; list = list->next) {
                if (strcmp(list->value, mount->dirname) == 0) {
                    break;
                }
            }
            if (!list) {
                continue;
            }
        }

        fd = qemu_open(mount->dirname, O_RDONLY);
        if (fd == -1) {
            error_setg_errno(errp, errno, "failed to open %s", mount->dirname);
            goto error;
        }

        /* we try to cull filesystems we know won't work in advance, but other
         * filesystems may not implement fsfreeze for less obvious reasons.
         * these will report EOPNOTSUPP. we simply ignore these when tallying
         * the number of frozen filesystems.
         * if a filesystem is mounted more than once (aka bind mount) a
         * consecutive attempt to freeze an already frozen filesystem will
         * return EBUSY.
         *
         * any other error means a failure to freeze a filesystem we
         * expect to be freezable, so return an error in those cases
         * and return system to thawed state.
         */
        ret = ioctl(fd, FIFREEZE);
        if (ret == -1) {
            if (errno != EOPNOTSUPP && errno != EBUSY) {
                error_setg_errno(errp, errno, "failed to freeze %s",
                                 mount->dirname);
                close(fd);
                goto error;
            }
        } else {
            i++;
        }
        close(fd);
    }

    free_fs_mount_list(&mounts);
    /* We may not issue any FIFREEZE here.
     * Just unset ga_state here and ready for the next call.
     */
    if (i == 0) {
        ga_unset_frozen(ga_state);
    }
    return i;

error:
    free_fs_mount_list(&mounts);
    qmp_guest_fsfreeze_thaw(NULL);
    return 0;
}