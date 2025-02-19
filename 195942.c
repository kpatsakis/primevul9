ssize_t qemu_sendv_packet_async(NetClientState *sender,
                                const struct iovec *iov, int iovcnt,
                                NetPacketSent *sent_cb)
{
    NetQueue *queue;
    size_t size = iov_size(iov, iovcnt);
    int ret;

    if (size > NET_BUFSIZE) {
        return size;
    }

    if (sender->link_down || !sender->peer) {
        return size;
    }

    /* Let filters handle the packet first */
    ret = filter_receive_iov(sender, NET_FILTER_DIRECTION_TX, sender,
                             QEMU_NET_PACKET_FLAG_NONE, iov, iovcnt, sent_cb);
    if (ret) {
        return ret;
    }

    ret = filter_receive_iov(sender->peer, NET_FILTER_DIRECTION_RX, sender,
                             QEMU_NET_PACKET_FLAG_NONE, iov, iovcnt, sent_cb);
    if (ret) {
        return ret;
    }

    queue = sender->peer->incoming_queue;

    return qemu_net_queue_send_iov(queue, sender,
                                   QEMU_NET_PACKET_FLAG_NONE,
                                   iov, iovcnt, sent_cb);
}