static uint64_t vmxnet3_get_mac_low(MACAddr *addr)
{
    return VMXNET3_MAKE_BYTE(0, addr->a[0]) |
           VMXNET3_MAKE_BYTE(1, addr->a[1]) |
           VMXNET3_MAKE_BYTE(2, addr->a[2]) |
           VMXNET3_MAKE_BYTE(3, addr->a[3]);
}