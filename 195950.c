void qemu_foreach_nic(qemu_nic_foreach func, void *opaque)
{
    NetClientState *nc;

    QTAILQ_FOREACH(nc, &net_clients, next) {
        if (nc->info->type == NET_CLIENT_DRIVER_NIC) {
            if (nc->queue_index == 0) {
                func(qemu_get_nic(nc), opaque);
            }
        }
    }
}