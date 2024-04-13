GuestNetworkInterfaceList *qmp_guest_network_get_interfaces(Error **errp)
{
    error_setg(errp, QERR_UNSUPPORTED);
    return NULL;
}