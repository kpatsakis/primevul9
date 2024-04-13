void qemu_del_net_client(NetClientState *nc)
{
    NetClientState *ncs[MAX_QUEUE_NUM];
    int queues, i;
    NetFilterState *nf, *next;

    assert(nc->info->type != NET_CLIENT_DRIVER_NIC);

    /* If the NetClientState belongs to a multiqueue backend, we will change all
     * other NetClientStates also.
     */
    queues = qemu_find_net_clients_except(nc->name, ncs,
                                          NET_CLIENT_DRIVER_NIC,
                                          MAX_QUEUE_NUM);
    assert(queues != 0);

    QTAILQ_FOREACH_SAFE(nf, &nc->filters, next, next) {
        object_unparent(OBJECT(nf));
    }

    /* If there is a peer NIC, delete and cleanup client, but do not free. */
    if (nc->peer && nc->peer->info->type == NET_CLIENT_DRIVER_NIC) {
        NICState *nic = qemu_get_nic(nc->peer);
        if (nic->peer_deleted) {
            return;
        }
        nic->peer_deleted = true;

        for (i = 0; i < queues; i++) {
            ncs[i]->peer->link_down = true;
        }

        if (nc->peer->info->link_status_changed) {
            nc->peer->info->link_status_changed(nc->peer);
        }

        for (i = 0; i < queues; i++) {
            qemu_cleanup_net_client(ncs[i]);
        }

        return;
    }

    for (i = 0; i < queues; i++) {
        qemu_cleanup_net_client(ncs[i]);
        qemu_free_net_client(ncs[i]);
    }
}