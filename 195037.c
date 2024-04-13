static uint32_t rtl8139_Config0_read(RTL8139State *s)
{
    uint32_t ret = s->Config0;

    DPRINTF("Config0 read val=0x%02x\n", ret);

    return ret;
}