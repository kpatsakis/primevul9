static int net_client_init1(const void *object, bool is_netdev, Error **errp)
{
    Netdev legacy = {0};
    const Netdev *netdev;
    const char *name;
    NetClientState *peer = NULL;

    if (is_netdev) {
        netdev = object;
        name = netdev->id;

        if (netdev->type == NET_CLIENT_DRIVER_NIC ||
            !net_client_init_fun[netdev->type]) {
            error_setg(errp, QERR_INVALID_PARAMETER_VALUE, "type",
                       "a netdev backend type");
            return -1;
        }
    } else {
        const NetLegacy *net = object;
        const NetLegacyOptions *opts = net->opts;
        legacy.id = net->id;
        netdev = &legacy;
        /* missing optional values have been initialized to "all bits zero" */
        name = net->has_id ? net->id : net->name;

        if (net->has_name) {
            warn_report("The 'name' parameter is deprecated, use 'id' instead");
        }

        /* Map the old options to the new flat type */
        switch (opts->type) {
        case NET_LEGACY_OPTIONS_TYPE_NONE:
            return 0; /* nothing to do */
        case NET_LEGACY_OPTIONS_TYPE_NIC:
            legacy.type = NET_CLIENT_DRIVER_NIC;
            legacy.u.nic = opts->u.nic;
            break;
        case NET_LEGACY_OPTIONS_TYPE_USER:
            legacy.type = NET_CLIENT_DRIVER_USER;
            legacy.u.user = opts->u.user;
            break;
        case NET_LEGACY_OPTIONS_TYPE_TAP:
            legacy.type = NET_CLIENT_DRIVER_TAP;
            legacy.u.tap = opts->u.tap;
            break;
        case NET_LEGACY_OPTIONS_TYPE_L2TPV3:
            legacy.type = NET_CLIENT_DRIVER_L2TPV3;
            legacy.u.l2tpv3 = opts->u.l2tpv3;
            break;
        case NET_LEGACY_OPTIONS_TYPE_SOCKET:
            legacy.type = NET_CLIENT_DRIVER_SOCKET;
            legacy.u.socket = opts->u.socket;
            break;
        case NET_LEGACY_OPTIONS_TYPE_VDE:
            legacy.type = NET_CLIENT_DRIVER_VDE;
            legacy.u.vde = opts->u.vde;
            break;
        case NET_LEGACY_OPTIONS_TYPE_BRIDGE:
            legacy.type = NET_CLIENT_DRIVER_BRIDGE;
            legacy.u.bridge = opts->u.bridge;
            break;
        case NET_LEGACY_OPTIONS_TYPE_NETMAP:
            legacy.type = NET_CLIENT_DRIVER_NETMAP;
            legacy.u.netmap = opts->u.netmap;
            break;
        case NET_LEGACY_OPTIONS_TYPE_VHOST_USER:
            legacy.type = NET_CLIENT_DRIVER_VHOST_USER;
            legacy.u.vhost_user = opts->u.vhost_user;
            break;
        default:
            abort();
        }

        if (!net_client_init_fun[netdev->type]) {
            error_setg(errp, QERR_INVALID_PARAMETER_VALUE, "type",
                       "a net backend type (maybe it is not compiled "
                       "into this binary)");
            return -1;
        }

        /* Do not add to a hub if it's a nic with a netdev= parameter. */
        if (netdev->type != NET_CLIENT_DRIVER_NIC ||
            !opts->u.nic.has_netdev) {
            peer = net_hub_add_port(0, NULL, NULL);
        }
    }

    if (net_client_init_fun[netdev->type](netdev, name, peer, errp) < 0) {
        /* FIXME drop when all init functions store an Error */
        if (errp && !*errp) {
            error_setg(errp, QERR_DEVICE_INIT_FAILED,
                       NetClientDriver_str(netdev->type));
        }
        return -1;
    }
    return 0;
}