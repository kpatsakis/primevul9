void qemu_flush_queued_packets(NetClientState *nc)
{
    qemu_flush_or_purge_queued_packets(nc, false);
}