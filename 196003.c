static void qemu_macaddr_set_used(MACAddr *macaddr)
{
    int index;

    for (index = 0x56; index < 0xFF; index++) {
        if (macaddr->a[5] == index) {
            mac_table[index]++;
        }
    }
}