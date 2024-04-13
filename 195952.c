static void qemu_macaddr_set_free(MACAddr *macaddr)
{
    int index;
    static const MACAddr base = { .a = { 0x52, 0x54, 0x00, 0x12, 0x34, 0 } };

    if (memcmp(macaddr->a, &base.a, (sizeof(base.a) - 1)) != 0) {
        return;
    }
    for (index = 0x56; index < 0xFF; index++) {
        if (macaddr->a[5] == index) {
            mac_table[index]--;
        }
    }
}