ssize_t qemu_net_queue_receive_iov(NetQueue *queue,
                                   const struct iovec *iov,
                                   int iovcnt)
{
    if (queue->delivering) {
        return 0;
    }

    return qemu_net_queue_deliver_iov(queue, NULL, 0, iov, iovcnt);
}