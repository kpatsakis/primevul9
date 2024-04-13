ssize_t qemu_net_queue_receive(NetQueue *queue,
                               const uint8_t *data,
                               size_t size)
{
    if (queue->delivering) {
        return 0;
    }

    return qemu_net_queue_deliver(queue, NULL, 0, data, size);
}