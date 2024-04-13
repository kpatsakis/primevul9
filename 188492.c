int64_t qmp_guest_fsfreeze_thaw(Error **errp)
{
    error_setg(errp, QERR_UNSUPPORTED);

    return 0;
}