void qemu_purge_queued_packets(NetClientState *nc)
{
    if (!nc->peer) {
        return;
    }

    qemu_net_queue_purge(nc->peer->incoming_queue, nc);
}