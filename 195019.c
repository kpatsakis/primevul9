static uint32_t rtl8139_Config4_read(RTL8139State *s)
{
    uint32_t ret = s->Config4;

    DPRINTF("Config4 read val=0x%02x\n", ret);

    return ret;
}