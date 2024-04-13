static void rtl8139_CpCmd_write(RTL8139State *s, uint32_t val)
{
    val &= 0xffff;

    DPRINTF("C+ command register write(w) val=0x%04x\n", val);

    s->cplus_enabled = 1;

    /* mask unwritable bits */
    val = SET_MASKED(val, 0xff84, s->CpCmd);

    s->CpCmd = val;
}