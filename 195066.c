static uint32_t rtl8139_RxConfig_read(RTL8139State *s)
{
    uint32_t ret = s->RxConfig;

    DPRINTF("RxConfig read val=0x%08x\n", ret);

    return ret;
}