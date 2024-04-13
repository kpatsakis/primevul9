static void rtl8139_Config0_write(RTL8139State *s, uint32_t val)
{
    val &= 0xff;

    DPRINTF("Config0 write val=0x%02x\n", val);

    if (!rtl8139_config_writable(s)) {
        return;
    }

    /* mask unwritable bits */
    val = SET_MASKED(val, 0xf8, s->Config0);

    s->Config0 = val;
}