static int mptsas_build_sgl(MPTSASState *s, MPTSASRequest *req, hwaddr addr)
{
    PCIDevice *pci = (PCIDevice *) s;
    hwaddr next_chain_addr;
    uint32_t left;
    hwaddr sgaddr;
    uint32_t chain_offset;

    chain_offset = req->scsi_io.ChainOffset;
    next_chain_addr = addr + chain_offset * sizeof(uint32_t);
    sgaddr = addr + sizeof(MPIMsgSCSIIORequest);
    pci_dma_sglist_init(&req->qsg, pci, 4);
    left = req->scsi_io.DataLength;

    for(;;) {
        dma_addr_t addr, len;
        uint32_t flags_and_length;

        flags_and_length = ldl_le_pci_dma(pci, sgaddr);
        len = flags_and_length & MPI_SGE_LENGTH_MASK;
        if ((flags_and_length & MPI_SGE_FLAGS_ELEMENT_TYPE_MASK)
            != MPI_SGE_FLAGS_SIMPLE_ELEMENT ||
            (!len &&
             !(flags_and_length & MPI_SGE_FLAGS_END_OF_LIST) &&
             !(flags_and_length & MPI_SGE_FLAGS_END_OF_BUFFER))) {
            return MPI_IOCSTATUS_INVALID_SGL;
        }

        len = MIN(len, left);
        if (!len) {
            /* We reached the desired transfer length, ignore extra
             * elements of the s/g list.
             */
            break;
        }

        addr = mptsas_ld_sg_base(s, flags_and_length, &sgaddr);
        qemu_sglist_add(&req->qsg, addr, len);
        left -= len;

        if (flags_and_length & MPI_SGE_FLAGS_END_OF_LIST) {
            break;
        }

        if (flags_and_length & MPI_SGE_FLAGS_LAST_ELEMENT) {
            if (!chain_offset) {
                break;
            }

            flags_and_length = ldl_le_pci_dma(pci, next_chain_addr);
            if ((flags_and_length & MPI_SGE_FLAGS_ELEMENT_TYPE_MASK)
                != MPI_SGE_FLAGS_CHAIN_ELEMENT) {
                return MPI_IOCSTATUS_INVALID_SGL;
            }

            sgaddr = mptsas_ld_sg_base(s, flags_and_length, &next_chain_addr);
            chain_offset =
                (flags_and_length & MPI_SGE_CHAIN_OFFSET_MASK) >> MPI_SGE_CHAIN_OFFSET_SHIFT;
            next_chain_addr = sgaddr + chain_offset * sizeof(uint32_t);
        }
    }
    return 0;
}