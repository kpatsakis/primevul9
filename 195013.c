static uint32_t rtl8139_Config3_read(RTL8139State *s)
{
    uint32_t ret = s->Config3;

    DPRINTF("Config3 read val=0x%02x\n", ret);

    return ret;
}