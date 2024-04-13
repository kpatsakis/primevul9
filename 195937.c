void *qemu_get_nic_opaque(NetClientState *nc)
{
    NICState *nic = qemu_get_nic(nc);

    return nic->opaque;
}