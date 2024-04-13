static int rtl8139_cp_receiver_enabled(RTL8139State *s)
{
    return s->CpCmd & CPlusRxEnb;
}