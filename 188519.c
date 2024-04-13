int64_t qmp_guest_fsfreeze_freeze_list(bool has_mountpoints,
                                       strList *mountpoints,
                                       Error **errp)
{
    error_setg(errp, QERR_UNSUPPORTED);

    return 0;
}