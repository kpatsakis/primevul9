vmxnet3_read_next_rx_descr(VMXNET3State *s, int qidx, int ridx,
                           struct Vmxnet3_RxDesc *dbuf, uint32_t *didx)
{
    PCIDevice *d = PCI_DEVICE(s);

    Vmxnet3Ring *ring = &s->rxq_descr[qidx].rx_ring[ridx];
    *didx = vmxnet3_ring_curr_cell_idx(ring);
    vmxnet3_ring_read_curr_cell(d, ring, dbuf);
    dbuf->addr = le64_to_cpu(dbuf->addr);
    dbuf->val1 = le32_to_cpu(dbuf->val1);
    dbuf->ext1 = le32_to_cpu(dbuf->ext1);
}