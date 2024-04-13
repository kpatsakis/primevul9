vmxnet3_inc_rx_consumption_counter(VMXNET3State *s, int qidx, int ridx)
{
    vmxnet3_ring_inc(&s->rxq_descr[qidx].rx_ring[ridx]);
}