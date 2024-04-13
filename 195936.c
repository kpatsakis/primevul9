void net_cleanup(void)
{
    NetClientState *nc;

    /* We may del multiple entries during qemu_del_net_client(),
     * so QTAILQ_FOREACH_SAFE() is also not safe here.
     */
    while (!QTAILQ_EMPTY(&net_clients)) {
        nc = QTAILQ_FIRST(&net_clients);
        if (nc->info->type == NET_CLIENT_DRIVER_NIC) {
            qemu_del_nic(qemu_get_nic(nc));
        } else {
            qemu_del_net_client(nc);
        }
    }

    qemu_del_vm_change_state_handler(net_change_state_entry);
}