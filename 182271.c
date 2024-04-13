static void vmxnet3_set_variable_mac(VMXNET3State *s, uint32_t h, uint32_t l)
{
    s->conf.macaddr.a[0] = VMXNET3_GET_BYTE(l,  0);
    s->conf.macaddr.a[1] = VMXNET3_GET_BYTE(l,  1);
    s->conf.macaddr.a[2] = VMXNET3_GET_BYTE(l,  2);
    s->conf.macaddr.a[3] = VMXNET3_GET_BYTE(l,  3);
    s->conf.macaddr.a[4] = VMXNET3_GET_BYTE(h, 0);
    s->conf.macaddr.a[5] = VMXNET3_GET_BYTE(h, 1);

    VMW_CFPRN("Variable MAC: " MAC_FMT, MAC_ARG(s->conf.macaddr.a));

    qemu_format_nic_info_str(qemu_get_queue(s->nic), s->conf.macaddr.a);
}