static void rtl8139_ioport_write(void *opaque, hwaddr addr,
                                 uint64_t val, unsigned size)
{
    switch (size) {
    case 1:
        rtl8139_io_writeb(opaque, addr, val);
        break;
    case 2:
        rtl8139_io_writew(opaque, addr, val);
        break;
    case 4:
        rtl8139_io_writel(opaque, addr, val);
        break;
    }
}