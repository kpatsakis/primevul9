static ssize_t filter_receive_iov(NetClientState *nc,
                                  NetFilterDirection direction,
                                  NetClientState *sender,
                                  unsigned flags,
                                  const struct iovec *iov,
                                  int iovcnt,
                                  NetPacketSent *sent_cb)
{
    ssize_t ret = 0;
    NetFilterState *nf = NULL;

    if (direction == NET_FILTER_DIRECTION_TX) {
        QTAILQ_FOREACH(nf, &nc->filters, next) {
            ret = qemu_netfilter_receive(nf, direction, sender, flags, iov,
                                         iovcnt, sent_cb);
            if (ret) {
                return ret;
            }
        }
    } else {
        QTAILQ_FOREACH_REVERSE(nf, &nc->filters, next) {
            ret = qemu_netfilter_receive(nf, direction, sender, flags, iov,
                                         iovcnt, sent_cb);
            if (ret) {
                return ret;
            }
        }
    }

    return ret;
}