vmxnet3_inc_tx_completion_counter(VMXNET3State *s, int qidx)
{
    vmxnet3_ring_inc(&s->txq_descr[qidx].comp_ring);
}