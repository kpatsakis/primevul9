void sdhci_uninitfn(SDHCIState *s)
{
    timer_del(s->insert_timer);
    timer_free(s->insert_timer);
    timer_del(s->transfer_timer);
    timer_free(s->transfer_timer);

    g_free(s->fifo_buffer);
    s->fifo_buffer = NULL;
}