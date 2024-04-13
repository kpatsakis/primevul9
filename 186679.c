static void mptsas_turbo_reply(MPTSASState *s, uint32_t msgctx)
{
    if (MPTSAS_FIFO_FULL(s, reply_post)) {
        mptsas_set_fault(s, MPI_IOCSTATUS_INSUFFICIENT_RESOURCES);
        return;
    }

    /* The reply is just the message context ID (bit 31 = clear). */
    MPTSAS_FIFO_PUT(s, reply_post, msgctx);

    s->intr_status |= MPI_HIS_REPLY_MESSAGE_INTERRUPT;
    mptsas_update_interrupt(s);
}