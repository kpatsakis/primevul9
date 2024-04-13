static uint64_t mptsas_mmio_read(void *opaque, hwaddr addr,
                                  unsigned size)
{
    MPTSASState *s = opaque;
    uint32_t ret = 0;

    switch (addr & ~3) {
    case MPI_DOORBELL_OFFSET:
        ret = mptsas_doorbell_read(s);
        break;

    case MPI_DIAGNOSTIC_OFFSET:
        ret = s->diagnostic;
        break;

    case MPI_HOST_INTERRUPT_STATUS_OFFSET:
        ret = s->intr_status;
        break;

    case MPI_HOST_INTERRUPT_MASK_OFFSET:
        ret = s->intr_mask;
        break;

    case MPI_REPLY_POST_FIFO_OFFSET:
        ret = mptsas_reply_post_read(s);
        break;

    default:
        trace_mptsas_mmio_unhandled_read(s, addr);
        break;
    }
    trace_mptsas_mmio_read(s, addr, ret);
    return ret;
}