vmxnet3_get_rx_ring_gen(VMXNET3State *s, int qidx, int ridx)
{
    return s->rxq_descr[qidx].rx_ring[ridx].gen;
}