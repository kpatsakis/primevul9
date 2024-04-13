static void rtl8139_RxConfig_write(RTL8139State *s, uint32_t val)
{
    DPRINTF("RxConfig write val=0x%08x\n", val);

    /* mask unwritable bits */
    val = SET_MASKED(val, 0xf0fc0040, s->RxConfig);

    s->RxConfig = val;

    /* reset buffer size and read/write pointers */
    rtl8139_reset_rxring(s, 8192 << ((s->RxConfig >> 11) & 0x3));

    DPRINTF("RxConfig write reset buffer size to %d\n", s->RxBufferSize);
}