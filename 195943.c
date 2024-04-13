NetClientState *qemu_get_peer(NetClientState *nc, int queue_index)
{
    assert(nc != NULL);
    NetClientState *ncs = nc + queue_index;
    return ncs->peer;
}