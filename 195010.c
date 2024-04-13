static int rtl8139_receiver_enabled(RTL8139State *s)
{
    return s->bChipCmdState & CmdRxEnb;
}