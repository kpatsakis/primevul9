static void vmxnet3_setup_rx_filtering(VMXNET3State *s)
{
    vmxnet3_update_rx_mode(s);
    vmxnet3_update_vlan_filters(s);
    vmxnet3_update_mcast_filters(s);
}