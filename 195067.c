static void rtl8139_IntrMask_write(RTL8139State *s, uint32_t val)
{
    DPRINTF("IntrMask write(w) val=0x%04x\n", val);

    /* mask unwritable bits */
    val = SET_MASKED(val, 0x1e00, s->IntrMask);

    s->IntrMask = val;

    rtl8139_update_irq(s);

}