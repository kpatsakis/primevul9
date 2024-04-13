static uint32_t rtl8139_IntrStatus_read(RTL8139State *s)
{
    uint32_t ret = s->IntrStatus;

    DPRINTF("IntrStatus read(w) val=0x%04x\n", ret);

#if 0

    /* reading ISR clears all interrupts */
    s->IntrStatus = 0;

    rtl8139_update_irq(s);

#endif

    return ret;
}