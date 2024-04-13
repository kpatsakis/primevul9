static bool rtl8139_cp_rx_valid(RTL8139State *s)
{
    return !(s->RxRingAddrLO == 0 && s->RxRingAddrHI == 0);
}