vmxnet3_can_receive(NetClientState *nc)
{
    VMXNET3State *s = qemu_get_nic_opaque(nc);
    return s->device_active &&
           VMXNET_FLAG_IS_SET(s->link_status_and_speed, VMXNET3_LINK_STATUS_UP);
}