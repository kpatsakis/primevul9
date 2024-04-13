static uint32_t rtl8139_Config5_read(RTL8139State *s)
{
    uint32_t ret = s->Config5;

    DPRINTF("Config5 read val=0x%02x\n", ret);

    return ret;
}