ssize_t qemu_send_packet(NetClientState *nc, const uint8_t *buf, int size)
{
    return qemu_send_packet_async(nc, buf, size, NULL);
}