void qemu_del_net_queue(NetQueue *queue)
{
    NetPacket *packet, *next;

    QTAILQ_FOREACH_SAFE(packet, &queue->packets, entry, next) {
        QTAILQ_REMOVE(&queue->packets, packet, entry);
        g_free(packet);
    }

    g_free(queue);
}