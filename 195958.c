static int net_init_client(void *dummy, QemuOpts *opts, Error **errp)
{
    return net_client_init(opts, false, errp);
}