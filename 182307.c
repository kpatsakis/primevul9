static void vmxnet3_deactivate_device(VMXNET3State *s)
{
    if (s->device_active) {
        VMW_CBPRN("Deactivating vmxnet3...");
        net_tx_pkt_reset(s->tx_pkt);
        net_tx_pkt_uninit(s->tx_pkt);
        net_rx_pkt_uninit(s->rx_pkt);
        s->device_active = false;
    }
}