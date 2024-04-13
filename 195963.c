qemu_sendv_packet(NetClientState *nc, const struct iovec *iov, int iovcnt)
{
    return qemu_sendv_packet_async(nc, iov, iovcnt, NULL);
}