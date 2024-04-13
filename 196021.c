NetClientState *qemu_find_netdev(const char *id)
{
    NetClientState *nc;

    QTAILQ_FOREACH(nc, &net_clients, next) {
        if (nc->info->type == NET_CLIENT_DRIVER_NIC)
            continue;
        if (!strcmp(nc->name, id)) {
            return nc;
        }
    }

    return NULL;
}