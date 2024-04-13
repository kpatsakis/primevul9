static void rtl8139_reset_rxring(RTL8139State *s, uint32_t bufferSize)
{
    s->RxBufferSize = bufferSize;
    s->RxBufPtr  = 0;
    s->RxBufAddr = 0;
}