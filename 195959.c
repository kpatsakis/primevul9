static ssize_t qemu_net_queue_deliver(NetQueue *queue,
                                      NetClientState *sender,
                                      unsigned flags,
                                      const uint8_t *data,
                                      size_t size)
{
    ssize_t ret = -1;
    struct iovec iov = {
        .iov_base = (void *)data,
        .iov_len = size
    };

    queue->delivering = 1;
    ret = queue->deliver(sender, flags, &iov, 1, queue->opaque);
    queue->delivering = 0;

    return ret;
}