void qemu_del_nic(NICState *nic)
{
    int i, queues = MAX(nic->conf->peers.queues, 1);

    qemu_macaddr_set_free(&nic->conf->macaddr);

    /* If this is a peer NIC and peer has already been deleted, free it now. */
    if (nic->peer_deleted) {
        for (i = 0; i < queues; i++) {
            qemu_free_net_client(qemu_get_subqueue(nic, i)->peer);
        }
    }

    for (i = queues - 1; i >= 0; i--) {
        NetClientState *nc = qemu_get_subqueue(nic, i);

        qemu_cleanup_net_client(nc);
        qemu_free_net_client(nc);
    }

    g_free(nic);
}