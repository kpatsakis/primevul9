static uint32_t rtl8139_Config1_read(RTL8139State *s)
{
    uint32_t ret = s->Config1;

    DPRINTF("Config1 read val=0x%02x\n", ret);

    return ret;
}