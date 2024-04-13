static void rtl8139_IntrStatus_write(RTL8139State *s, uint32_t val)
{
    DPRINTF("IntrStatus write(w) val=0x%04x\n", val);

#if 0

    /* writing to ISR has no effect */

    return;

#else
    uint16_t newStatus = s->IntrStatus & ~val;

    /* mask unwritable bits */
    newStatus = SET_MASKED(newStatus, 0x1e00, s->IntrStatus);

    /* writing 1 to interrupt status register bit clears it */
    s->IntrStatus = 0;
    rtl8139_update_irq(s);

    s->IntrStatus = newStatus;
    rtl8139_set_next_tctr_time(s);
    rtl8139_update_irq(s);

#endif
}