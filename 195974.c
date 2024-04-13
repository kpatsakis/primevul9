NetClientState *qemu_get_subqueue(NICState *nic, int queue_index)
{
    return nic->ncs + queue_index;
}