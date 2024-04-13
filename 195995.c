static int net_init_netdev(void *dummy, QemuOpts *opts, Error **errp)
{
    const char *type = qemu_opt_get(opts, "type");

    if (type && is_help_option(type)) {
        show_netdevs();
        exit(0);
    }
    return net_client_init(opts, true, errp);
}