int net_init_clients(Error **errp)
{
    net_change_state_entry =
        qemu_add_vm_change_state_handler(net_vm_change_state_handler, NULL);

    QTAILQ_INIT(&net_clients);

    if (qemu_opts_foreach(qemu_find_opts("netdev"),
                          net_init_netdev, NULL, errp)) {
        return -1;
    }

    if (qemu_opts_foreach(qemu_find_opts("nic"), net_param_nic, NULL, errp)) {
        return -1;
    }

    if (qemu_opts_foreach(qemu_find_opts("net"), net_init_client, NULL, errp)) {
        return -1;
    }

    return 0;
}