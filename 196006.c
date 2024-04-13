void hmp_info_network(Monitor *mon, const QDict *qdict)
{
    NetClientState *nc, *peer;
    NetClientDriver type;

    net_hub_info(mon);

    QTAILQ_FOREACH(nc, &net_clients, next) {
        peer = nc->peer;
        type = nc->info->type;

        /* Skip if already printed in hub info */
        if (net_hub_id_for_client(nc, NULL) == 0) {
            continue;
        }

        if (!peer || type == NET_CLIENT_DRIVER_NIC) {
            print_net_client(mon, nc);
        } /* else it's a netdev connected to a NIC, printed with the NIC */
        if (peer && type == NET_CLIENT_DRIVER_NIC) {
            monitor_printf(mon, " \\ ");
            print_net_client(mon, peer);
        }
    }
}