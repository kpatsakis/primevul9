static int net_client_init1(const Netdev *netdev, bool is_netdev, Error **errp)
{
    NetClientState *peer = NULL;
    NetClientState *nc;

    if (is_netdev) {
        if (netdev->type == NET_CLIENT_DRIVER_NIC ||
            !net_client_init_fun[netdev->type]) {
            error_setg(errp, QERR_INVALID_PARAMETER_VALUE, "type",
                       "a netdev backend type");
            return -1;
        }
    } else {
        if (netdev->type == NET_CLIENT_DRIVER_NONE) {
            return 0; /* nothing to do */
        }
        if (netdev->type == NET_CLIENT_DRIVER_HUBPORT ||
            !net_client_init_fun[netdev->type]) {
            error_setg(errp, QERR_INVALID_PARAMETER_VALUE, "type",
                       "a net backend type (maybe it is not compiled "
                       "into this binary)");
            return -1;
        }

        /* Do not add to a hub if it's a nic with a netdev= parameter. */
        if (netdev->type != NET_CLIENT_DRIVER_NIC ||
            !netdev->u.nic.has_netdev) {
            peer = net_hub_add_port(0, NULL, NULL);
        }
    }

    nc = qemu_find_netdev(netdev->id);
    if (nc) {
        error_setg(errp, "Duplicate ID '%s'", netdev->id);
        return -1;
    }

    if (net_client_init_fun[netdev->type](netdev, netdev->id, peer, errp) < 0) {
        /* FIXME drop when all init functions store an Error */
        if (errp && !*errp) {
            error_setg(errp, "Device '%s' could not be initialized",
                       NetClientDriver_str(netdev->type));
        }
        return -1;
    }

    if (is_netdev) {
        nc = qemu_find_netdev(netdev->id);
        assert(nc);
        nc->is_netdev = true;
    }

    return 0;
}