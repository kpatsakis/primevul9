static void vmxnet3_set_link_status(NetClientState *nc)
{
    VMXNET3State *s = qemu_get_nic_opaque(nc);

    if (nc->link_down) {
        s->link_status_and_speed &= ~VMXNET3_LINK_STATUS_UP;
    } else {
        s->link_status_and_speed |= VMXNET3_LINK_STATUS_UP;
    }

    vmxnet3_set_events(s, VMXNET3_ECR_LINK);
    vmxnet3_trigger_interrupt(s, s->event_int_idx);
}