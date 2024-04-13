void qemu_net_queue_purge(NetQueue *queue, NetClientState *from)
{
    NetPacket *packet, *next;

    QTAILQ_FOREACH_SAFE(packet, &queue->packets, entry, next) {
        if (packet->sender == from) {
            QTAILQ_REMOVE(&queue->packets, packet, entry);
            queue->nq_count--;
            if (packet->sent_cb) {
                packet->sent_cb(packet->sender, 0);
            }
            g_free(packet);
        }
    }
}