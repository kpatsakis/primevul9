ssize_t qemu_send_packet_raw(NetClientState *nc, const uint8_t *buf, int size)
{
    return qemu_send_packet_async_with_flags(nc, QEMU_NET_PACKET_FLAG_RAW,
                                             buf, size, NULL);
}