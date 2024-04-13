static void rtl8139_BasicModeStatus_write(RTL8139State *s, uint32_t val)
{
    val &= 0xffff;

    DPRINTF("BasicModeStatus register write(w) val=0x%04x\n", val);

    /* mask unwritable bits */
    val = SET_MASKED(val, 0xff3f, s->BasicModeStatus);

    s->BasicModeStatus = val;
}