static int qemu_macaddr_get_free(void)
{
    int index;

    for (index = 0x56; index < 0xFF; index++) {
        if (mac_table[index] == 0) {
            return index;
        }
    }

    return -1;
}