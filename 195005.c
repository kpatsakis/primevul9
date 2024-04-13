static void rtl8139_ChipCmd_write(RTL8139State *s, uint32_t val)
{
    DeviceState *d = DEVICE(s);

    val &= 0xff;

    DPRINTF("ChipCmd write val=0x%08x\n", val);

    if (val & CmdReset)
    {
        DPRINTF("ChipCmd reset\n");
        rtl8139_reset(d);
    }
    if (val & CmdRxEnb)
    {
        DPRINTF("ChipCmd enable receiver\n");

        s->currCPlusRxDesc = 0;
    }
    if (val & CmdTxEnb)
    {
        DPRINTF("ChipCmd enable transmitter\n");

        s->currCPlusTxDesc = 0;
    }

    /* mask unwritable bits */
    val = SET_MASKED(val, 0xe3, s->bChipCmdState);

    /* Deassert reset pin before next read */
    val &= ~CmdReset;

    s->bChipCmdState = val;
}