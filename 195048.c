static uint32_t rtl8139_RxBuf_read(RTL8139State *s)
{
    uint32_t ret = s->RxBuf;

    DPRINTF("RxBuf read val=0x%08x\n", ret);

    return ret;
}