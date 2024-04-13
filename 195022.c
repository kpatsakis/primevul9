static uint32_t rtl8139_RxBufAddr_read(RTL8139State *s)
{
    /* this value is NOT off by 16 */
    uint32_t ret = s->RxBufAddr;

    DPRINTF("RxBufAddr read val=0x%04x\n", ret);

    return ret;
}