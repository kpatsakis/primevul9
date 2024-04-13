static int rtl8139_cp_transmitter_enabled(RTL8139State *s)
{
    return s->CpCmd & CPlusTxEnb;
}