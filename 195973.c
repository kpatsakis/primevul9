void qmp_netdev_del(const char *id, Error **errp)
{
    NetClientState *nc;
    QemuOpts *opts;

    nc = qemu_find_netdev(id);
    if (!nc) {
        error_set(errp, ERROR_CLASS_DEVICE_NOT_FOUND,
                  "Device '%s' not found", id);
        return;
    }

    if (!nc->is_netdev) {
        error_setg(errp, "Device '%s' is not a netdev", id);
        return;
    }

    qemu_del_net_client(nc);

    /*
     * Wart: we need to delete the QemuOpts associated with netdevs
     * created via CLI or HMP, to avoid bogus "Duplicate ID" errors in
     * HMP netdev_add.
     */
    opts = qemu_opts_find(qemu_find_opts("netdev"), id);
    if (opts) {
        qemu_opts_del(opts);
    }
}