static uint32_t rtl8139_ChipCmd_read(RTL8139State *s)
{
    uint32_t ret = s->bChipCmdState;

    if (rtl8139_RxBufferEmpty(s))
        ret |= RxBufEmpty;

    DPRINTF("ChipCmd read val=0x%04x\n", ret);

    return ret;
}