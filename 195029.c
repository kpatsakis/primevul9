static void rtl8139_MultiIntr_write(RTL8139State *s, uint32_t val)
{
    DPRINTF("MultiIntr write(w) val=0x%04x\n", val);

    /* mask unwritable bits */
    val = SET_MASKED(val, 0xf000, s->MultiIntr);

    s->MultiIntr = val;
}