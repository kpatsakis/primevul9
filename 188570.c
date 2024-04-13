int64_t qmp_guest_set_vcpus(GuestLogicalProcessorList *vcpus, Error **errp)
{
    error_setg(errp, QERR_UNSUPPORTED);
    return -1;
}