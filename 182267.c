vmxnet3_inc_rx_completion_counter(VMXNET3State *s, int qidx)
{
    vmxnet3_ring_inc(&s->rxq_descr[qidx].comp_ring);
}