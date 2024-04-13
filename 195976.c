static ssize_t qemu_net_queue_deliver_iov(NetQueue *queue,
                                          NetClientState *sender,
                                          unsigned flags,
                                          const struct iovec *iov,
                                          int iovcnt)
{
    ssize_t ret = -1;

    queue->delivering = 1;
    ret = queue->deliver(sender, flags, iov, iovcnt, queue->opaque);
    queue->delivering = 0;

    return ret;
}