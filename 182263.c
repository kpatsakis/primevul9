vmxnet3_pci_dma_write_rxcd(PCIDevice *pcidev, dma_addr_t pa,
                           struct Vmxnet3_RxCompDesc *rxcd)
{
    rxcd->val1 = cpu_to_le32(rxcd->val1);
    rxcd->val2 = cpu_to_le32(rxcd->val2);
    rxcd->val3 = cpu_to_le32(rxcd->val3);
    pci_dma_write(pcidev, pa, rxcd, sizeof(*rxcd));
}