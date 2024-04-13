static ssize_t rtl8139_receive(NetClientState *nc, const uint8_t *buf, size_t size)
{
    return rtl8139_do_receive(nc, buf, size, 1);
}