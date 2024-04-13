void qmp_guest_set_user_password(const char *username,
                                 const char *password,
                                 bool crypted,
                                 Error **errp)
{
    error_setg(errp, QERR_UNSUPPORTED);
}