static void rtl8139_Config1_write(RTL8139State *s, uint32_t val)
{
    val &= 0xff;

    DPRINTF("Config1 write val=0x%02x\n", val);

    if (!rtl8139_config_writable(s)) {
        return;
    }

    /* mask unwritable bits */
    val = SET_MASKED(val, 0xC, s->Config1);

    s->Config1 = val;
}