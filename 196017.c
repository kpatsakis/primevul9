void qmp_netdev_add(Netdev *netdev, Error **errp)
{
    if (!id_wellformed(netdev->id)) {
        error_setg(errp, QERR_INVALID_PARAMETER_VALUE, "id", "an identifier");
        return;
    }

    net_client_init1(netdev, true, errp);
}