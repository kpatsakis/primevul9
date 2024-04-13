void qemu_macaddr_default_if_unset(MACAddr *macaddr)
{
    static const MACAddr zero = { .a = { 0,0,0,0,0,0 } };
    static const MACAddr base = { .a = { 0x52, 0x54, 0x00, 0x12, 0x34, 0 } };

    if (memcmp(macaddr, &zero, sizeof(zero)) != 0) {
        if (memcmp(macaddr->a, &base.a, (sizeof(base.a) - 1)) != 0) {
            return;
        } else {
            qemu_macaddr_set_used(macaddr);
            return;
        }
    }

    macaddr->a[0] = 0x52;
    macaddr->a[1] = 0x54;
    macaddr->a[2] = 0x00;
    macaddr->a[3] = 0x12;
    macaddr->a[4] = 0x34;
    macaddr->a[5] = qemu_macaddr_get_free();
    qemu_macaddr_set_used(macaddr);
}