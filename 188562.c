GuestFilesystemInfoList *qmp_guest_get_fsinfo(Error **errp)
{
    error_setg(errp, QERR_UNSUPPORTED);
    return NULL;
}