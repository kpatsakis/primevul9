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

    opts = qemu_opts_find(qemu_find_opts_err("netdev", NULL), id);
    if (!opts) {
        error_setg(errp, "Device '%s' is not a netdev", id);
        return;
    }

    qemu_del_net_client(nc);
    qemu_opts_del(opts);
}