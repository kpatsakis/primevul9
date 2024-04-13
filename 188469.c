GuestFsfreezeStatus qmp_guest_fsfreeze_status(Error **errp)
{
    error_setg(errp, QERR_UNSUPPORTED);

    return 0;
}