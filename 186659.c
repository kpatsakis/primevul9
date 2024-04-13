static void mptsas_interrupt_status_write(MPTSASState *s)
{
    switch (s->doorbell_state) {
    case DOORBELL_NONE:
    case DOORBELL_WRITE:
        s->intr_status &= ~MPI_HIS_DOORBELL_INTERRUPT;
        break;

    case DOORBELL_READ:
        /* The reply can be read continuously, so leave the interrupt up.  */
        assert(s->intr_status & MPI_HIS_DOORBELL_INTERRUPT);
        if (s->doorbell_reply_idx == s->doorbell_reply_size) {
            s->doorbell_state = DOORBELL_NONE;
        }
        break;

    default:
        abort();
    }
    mptsas_update_interrupt(s);
}