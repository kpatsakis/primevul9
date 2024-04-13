void qemu_send_packet(NetClientState *nc, const uint8_t *buf, int size)
{
    qemu_send_packet_async(nc, buf, size, NULL);
}