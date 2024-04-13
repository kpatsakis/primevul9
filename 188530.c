GuestUserList *qmp_guest_get_users(Error **errp)
{
    error_setg(errp, QERR_UNSUPPORTED);
    return NULL;
}