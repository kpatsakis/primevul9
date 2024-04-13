static void rtl8139_Config4_write(RTL8139State *s, uint32_t val)
{
    val &= 0xff;

    DPRINTF("Config4 write val=0x%02x\n", val);

    if (!rtl8139_config_writable(s)) {
        return;
    }

    /* mask unwritable bits */
    val = SET_MASKED(val, 0x0a, s->Config4);

    s->Config4 = val;
}