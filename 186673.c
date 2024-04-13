static void mptsas_post_reply(MPTSASState *s, MPIDefaultReply *reply)
{
    PCIDevice *pci = (PCIDevice *) s;
    uint32_t addr_lo;

    if (MPTSAS_FIFO_EMPTY(s, reply_free) || MPTSAS_FIFO_FULL(s, reply_post)) {
        mptsas_set_fault(s, MPI_IOCSTATUS_INSUFFICIENT_RESOURCES);
        return;
    }

    addr_lo = MPTSAS_FIFO_GET(s, reply_free);

    pci_dma_write(pci, addr_lo | s->host_mfa_high_addr, reply,
                  MIN(s->reply_frame_size, 4 * reply->MsgLength));

    MPTSAS_FIFO_PUT(s, reply_post, MPI_ADDRESS_REPLY_A_BIT | (addr_lo >> 1));

    s->intr_status |= MPI_HIS_REPLY_MESSAGE_INTERRUPT;
    if (s->doorbell_state == DOORBELL_WRITE) {
        s->doorbell_state = DOORBELL_NONE;
        s->intr_status |= MPI_HIS_DOORBELL_INTERRUPT;
    }
    mptsas_update_interrupt(s);
}