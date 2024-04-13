static int rtl8139_RxBufferEmpty(RTL8139State *s)
{
    int unread = MOD2(s->RxBufferSize + s->RxBufAddr - s->RxBufPtr, s->RxBufferSize);

    if (unread != 0)
    {
        DPRINTF("receiver buffer data available 0x%04x\n", unread);
        return 0;
    }

    DPRINTF("receiver buffer is empty\n");

    return 1;
}