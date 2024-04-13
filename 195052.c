static void rtl8139_timer(void *opaque)
{
    RTL8139State *s = opaque;

    if (!s->clock_enabled)
    {
        DPRINTF(">>> timer: clock is not running\n");
        return;
    }

    s->IntrStatus |= PCSTimeout;
    rtl8139_update_irq(s);
    rtl8139_set_next_tctr_time(s);
}