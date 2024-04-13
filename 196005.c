NetClientState *qemu_new_net_client(NetClientInfo *info,
                                    NetClientState *peer,
                                    const char *model,
                                    const char *name)
{
    NetClientState *nc;

    assert(info->size >= sizeof(NetClientState));

    nc = g_malloc0(info->size);
    qemu_net_client_setup(nc, info, peer, model, name,
                          qemu_net_client_destructor);

    return nc;
}