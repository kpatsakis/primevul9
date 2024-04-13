GuestLogicalProcessorList *qmp_guest_get_vcpus(Error **errp)
{
    error_setg(errp, QERR_UNSUPPORTED);
    return NULL;
}