static int net_init_netdev(void *dummy, QemuOpts *opts, Error **errp)
{
    return net_client_init(opts, true, errp);
}