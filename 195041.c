static void rtl8139_RxBuf_write(RTL8139State *s, uint32_t val)
{
    DPRINTF("RxBuf write val=0x%08x\n", val);

    s->RxBuf = val;

    /* may need to reset rxring here */
}