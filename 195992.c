ssize_t qemu_net_queue_send(NetQueue *queue,
                            NetClientState *sender,
                            unsigned flags,
                            const uint8_t *data,
                            size_t size,
                            NetPacketSent *sent_cb)
{
    ssize_t ret;

    if (queue->delivering || !qemu_can_send_packet(sender)) {
        qemu_net_queue_append(queue, sender, flags, data, size, sent_cb);
        return 0;
    }

    ret = qemu_net_queue_deliver(queue, sender, flags, data, size);
    if (ret == 0) {
        qemu_net_queue_append(queue, sender, flags, data, size, sent_cb);
        return 0;
    }

    qemu_net_queue_flush(queue);

    return ret;
}