vmxnet3_dec_rx_completion_counter(VMXNET3State *s, int qidx)
{
    vmxnet3_ring_dec(&s->rxq_descr[qidx].comp_ring);
}