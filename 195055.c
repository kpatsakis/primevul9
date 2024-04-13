static void rtl8139_Config5_write(RTL8139State *s, uint32_t val)
{
    val &= 0xff;

    DPRINTF("Config5 write val=0x%02x\n", val);

    /* mask unwritable bits */
    val = SET_MASKED(val, 0x80, s->Config5);

    s->Config5 = val;
}