static void rtl8139_set_link_status(NetClientState *nc)
{
    RTL8139State *s = qemu_get_nic_opaque(nc);

    if (nc->link_down) {
        s->BasicModeStatus &= ~0x04;
    } else {
        s->BasicModeStatus |= 0x04;
    }

    s->IntrStatus |= RxUnderrun;
    rtl8139_update_irq(s);
}