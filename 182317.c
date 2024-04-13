vmxnet3_revert_rxc_descr(VMXNET3State *s, int qidx)
{
    vmxnet3_dec_rx_completion_counter(s, qidx);
}