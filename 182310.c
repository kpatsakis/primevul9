static uint64_t vmxnet3_get_mac_high(MACAddr *addr)
{
    return VMXNET3_MAKE_BYTE(0, addr->a[4]) |
           VMXNET3_MAKE_BYTE(1, addr->a[5]);
}