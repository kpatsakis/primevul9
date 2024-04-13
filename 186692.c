static dma_addr_t mptsas_ld_sg_base(MPTSASState *s, uint32_t flags_and_length,
                                    dma_addr_t *sgaddr)
{
    PCIDevice *pci = (PCIDevice *) s;
    dma_addr_t addr;

    if (flags_and_length & MPI_SGE_FLAGS_64_BIT_ADDRESSING) {
        addr = ldq_le_pci_dma(pci, *sgaddr + 4);
        *sgaddr += 12;
    } else {
        addr = ldl_le_pci_dma(pci, *sgaddr + 4);
        *sgaddr += 8;
    }
    return addr;
}