ssize_t qemu_receive_packet_iov(NetClientState *nc, const struct iovec *iov,
                                int iovcnt)
{
    if (!qemu_can_receive_packet(nc)) {
        return 0;
    }

    return qemu_net_queue_receive_iov(nc->incoming_queue, iov, iovcnt);
}