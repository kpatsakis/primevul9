static void mptsas_soft_reset(MPTSASState *s)
{
    uint32_t save_mask;

    trace_mptsas_reset(s);

    /* Temporarily disable interrupts */
    save_mask = s->intr_mask;
    s->intr_mask = MPI_HIM_DIM | MPI_HIM_RIM;
    mptsas_update_interrupt(s);

    qbus_reset_all(BUS(&s->bus));
    s->intr_status = 0;
    s->intr_mask = save_mask;

    s->reply_free_tail = 0;
    s->reply_free_head = 0;
    s->reply_post_tail = 0;
    s->reply_post_head = 0;
    s->request_post_tail = 0;
    s->request_post_head = 0;
    qemu_bh_cancel(s->request_bh);

    s->state = MPI_IOC_STATE_READY;
}