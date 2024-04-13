static uint32_t mptsas_reply_post_read(MPTSASState *s)
{
    uint32_t ret;

    if (!MPTSAS_FIFO_EMPTY(s, reply_post)) {
        ret = MPTSAS_FIFO_GET(s, reply_post);
    } else {
        ret = -1;
        s->intr_status &= ~MPI_HIS_REPLY_MESSAGE_INTERRUPT;
        mptsas_update_interrupt(s);
    }

    return ret;
}