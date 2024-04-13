static void mptsas_mmio_write(void *opaque, hwaddr addr,
                               uint64_t val, unsigned size)
{
    MPTSASState *s = opaque;

    trace_mptsas_mmio_write(s, addr, val);
    switch (addr) {
    case MPI_DOORBELL_OFFSET:
        mptsas_doorbell_write(s, val);
        break;

    case MPI_WRITE_SEQUENCE_OFFSET:
        mptsas_write_sequence_write(s, val);
        break;

    case MPI_DIAGNOSTIC_OFFSET:
        if (val & MPI_DIAG_RESET_ADAPTER) {
            mptsas_hard_reset(s);
        }
        break;

    case MPI_HOST_INTERRUPT_STATUS_OFFSET:
        mptsas_interrupt_status_write(s);
        break;

    case MPI_HOST_INTERRUPT_MASK_OFFSET:
        s->intr_mask = val & (MPI_HIM_RIM | MPI_HIM_DIM);
        mptsas_update_interrupt(s);
        break;

    case MPI_REQUEST_POST_FIFO_OFFSET:
        if (MPTSAS_FIFO_FULL(s, request_post)) {
            mptsas_set_fault(s, MPI_IOCSTATUS_INSUFFICIENT_RESOURCES);
        } else {
            MPTSAS_FIFO_PUT(s, request_post, val & ~0x03);
            qemu_bh_schedule(s->request_bh);
        }
        break;

    case MPI_REPLY_FREE_FIFO_OFFSET:
        if (MPTSAS_FIFO_FULL(s, reply_free)) {
            mptsas_set_fault(s, MPI_IOCSTATUS_INSUFFICIENT_RESOURCES);
        } else {
            MPTSAS_FIFO_PUT(s, reply_free, val);
        }
        break;

    default:
        trace_mptsas_mmio_unhandled_write(s, addr, val);
        break;
    }
}