ssize_t qemu_receive_packet(NetClientState *nc, const uint8_t *buf, int size)
{
    if (!qemu_can_receive_packet(nc)) {
        return 0;
    }

    return qemu_net_queue_receive(nc->incoming_queue, buf, size);
}