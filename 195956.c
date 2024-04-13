int qemu_find_net_clients_except(const char *id, NetClientState **ncs,
                                 NetClientDriver type, int max)
{
    NetClientState *nc;
    int ret = 0;

    QTAILQ_FOREACH(nc, &net_clients, next) {
        if (nc->info->type == type) {
            continue;
        }
        if (!id || !strcmp(nc->name, id)) {
            if (ret < max) {
                ncs[ret] = nc;
            }
            ret++;
        }
    }

    return ret;
}