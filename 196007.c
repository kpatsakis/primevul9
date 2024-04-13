NICState *qemu_new_nic(NetClientInfo *info,
                       NICConf *conf,
                       const char *model,
                       const char *name,
                       void *opaque)
{
    NetClientState **peers = conf->peers.ncs;
    NICState *nic;
    int i, queues = MAX(1, conf->peers.queues);

    assert(info->type == NET_CLIENT_DRIVER_NIC);
    assert(info->size >= sizeof(NICState));

    nic = g_malloc0(info->size + sizeof(NetClientState) * queues);
    nic->ncs = (void *)nic + info->size;
    nic->conf = conf;
    nic->opaque = opaque;

    for (i = 0; i < queues; i++) {
        qemu_net_client_setup(&nic->ncs[i], info, peers[i], model, name,
                              NULL);
        nic->ncs[i].queue_index = i;
    }

    return nic;
}