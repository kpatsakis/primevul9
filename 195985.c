NetClientState *qemu_get_queue(NICState *nic)
{
    return qemu_get_subqueue(nic, 0);
}