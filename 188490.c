qmp_guest_fstrim(bool has_minimum, int64_t minimum, Error **errp)
{
    error_setg(errp, QERR_UNSUPPORTED);
    return NULL;
}