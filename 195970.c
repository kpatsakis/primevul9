void qmp_set_link(const char *name, bool up, Error **errp)
{
    NetClientState *ncs[MAX_QUEUE_NUM];
    NetClientState *nc;
    int queues, i;

    queues = qemu_find_net_clients_except(name, ncs,
                                          NET_CLIENT_DRIVER__MAX,
                                          MAX_QUEUE_NUM);

    if (queues == 0) {
        error_set(errp, ERROR_CLASS_DEVICE_NOT_FOUND,
                  "Device '%s' not found", name);
        return;
    }
    nc = ncs[0];

    for (i = 0; i < queues; i++) {
        ncs[i]->link_down = !up;
    }

    if (nc->info->link_status_changed) {
        nc->info->link_status_changed(nc);
    }

    if (nc->peer) {
        /* Change peer link only if the peer is NIC and then notify peer.
         * If the peer is a HUBPORT or a backend, we do not change the
         * link status.
         *
         * This behavior is compatible with qemu hubs where there could be
         * multiple clients that can still communicate with each other in
         * disconnected mode. For now maintain this compatibility.
         */
        if (nc->peer->info->type == NET_CLIENT_DRIVER_NIC) {
            for (i = 0; i < queues; i++) {
                ncs[i]->peer->link_down = !up;
            }
        }
        if (nc->peer->info->link_status_changed) {
            nc->peer->info->link_status_changed(nc->peer);
        }
    }
}