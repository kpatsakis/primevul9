static void RTL8139TallyCounters_dma_write(RTL8139State *s, dma_addr_t tc_addr)
{
    PCIDevice *d = PCI_DEVICE(s);
    RTL8139TallyCounters *tally_counters = &s->tally_counters;
    uint16_t val16;
    uint32_t val32;
    uint64_t val64;

    val64 = cpu_to_le64(tally_counters->TxOk);
    pci_dma_write(d, tc_addr + 0,     (uint8_t *)&val64, 8);

    val64 = cpu_to_le64(tally_counters->RxOk);
    pci_dma_write(d, tc_addr + 8,     (uint8_t *)&val64, 8);

    val64 = cpu_to_le64(tally_counters->TxERR);
    pci_dma_write(d, tc_addr + 16,    (uint8_t *)&val64, 8);

    val32 = cpu_to_le32(tally_counters->RxERR);
    pci_dma_write(d, tc_addr + 24,    (uint8_t *)&val32, 4);

    val16 = cpu_to_le16(tally_counters->MissPkt);
    pci_dma_write(d, tc_addr + 28,    (uint8_t *)&val16, 2);

    val16 = cpu_to_le16(tally_counters->FAE);
    pci_dma_write(d, tc_addr + 30,    (uint8_t *)&val16, 2);

    val32 = cpu_to_le32(tally_counters->Tx1Col);
    pci_dma_write(d, tc_addr + 32,    (uint8_t *)&val32, 4);

    val32 = cpu_to_le32(tally_counters->TxMCol);
    pci_dma_write(d, tc_addr + 36,    (uint8_t *)&val32, 4);

    val64 = cpu_to_le64(tally_counters->RxOkPhy);
    pci_dma_write(d, tc_addr + 40,    (uint8_t *)&val64, 8);

    val64 = cpu_to_le64(tally_counters->RxOkBrd);
    pci_dma_write(d, tc_addr + 48,    (uint8_t *)&val64, 8);

    val32 = cpu_to_le32(tally_counters->RxOkMul);
    pci_dma_write(d, tc_addr + 56,    (uint8_t *)&val32, 4);

    val16 = cpu_to_le16(tally_counters->TxAbt);
    pci_dma_write(d, tc_addr + 60,    (uint8_t *)&val16, 2);

    val16 = cpu_to_le16(tally_counters->TxUndrn);
    pci_dma_write(d, tc_addr + 62,    (uint8_t *)&val16, 2);
}