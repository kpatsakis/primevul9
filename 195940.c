void netdev_add(QemuOpts *opts, Error **errp)
{
    net_client_init(opts, true, errp);
}