static int rtl8139_transmitter_enabled(RTL8139State *s)
{
    return s->bChipCmdState & CmdTxEnb;
}