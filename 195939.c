static ssize_t filter_receive(NetClientState *nc,
                              NetFilterDirection direction,
                              NetClientState *sender,
                              unsigned flags,
                              const uint8_t *data,
                              size_t size,
                              NetPacketSent *sent_cb)
{
    struct iovec iov = {
        .iov_base = (void *)data,
        .iov_len = size
    };

    return filter_receive_iov(nc, direction, sender, flags, &iov, 1, sent_cb);
}