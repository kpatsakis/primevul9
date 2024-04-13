static void qemu_cleanup_net_client(NetClientState *nc)
{
    QTAILQ_REMOVE(&net_clients, nc, next);

    if (nc->info->cleanup) {
        nc->info->cleanup(nc);
    }
}