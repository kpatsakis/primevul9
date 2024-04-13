static uint64_t rtl8139_ioport_read(void *opaque, hwaddr addr,
                                    unsigned size)
{
    switch (size) {
    case 1:
        return rtl8139_io_readb(opaque, addr);
    case 2:
        return rtl8139_io_readw(opaque, addr);
    case 4:
        return rtl8139_io_readl(opaque, addr);
    }

    return -1;
}