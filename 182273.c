vmxnet3_ring_read_curr_txdesc(PCIDevice *pcidev, Vmxnet3Ring *ring,
                              struct Vmxnet3_TxDesc *txd)
{
    vmxnet3_ring_read_curr_cell(pcidev, ring, txd);
    txd->addr = le64_to_cpu(txd->addr);
    txd->val1 = le32_to_cpu(txd->val1);
    txd->val2 = le32_to_cpu(txd->val2);
}