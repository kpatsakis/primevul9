NICState *qemu_get_nic(NetClientState *nc)
{
    NetClientState *nc0 = nc - nc->queue_index;

    return (NICState *)((void *)nc0 - nc->info->size);
}