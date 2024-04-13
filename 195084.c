static bool rtl8139_can_receive(NetClientState *nc)
{
    RTL8139State *s = qemu_get_nic_opaque(nc);
    int avail;

    /* Receive (drop) packets if card is disabled.  */
    if (!s->clock_enabled) {
        return true;
    }
    if (!rtl8139_receiver_enabled(s)) {
        return true;
    }

    if (rtl8139_cp_receiver_enabled(s) && rtl8139_cp_rx_valid(s)) {
        /* ??? Flow control not implemented in c+ mode.
           This is a hack to work around slirp deficiencies anyway.  */
        return true;
    }

    avail = MOD2(s->RxBufferSize + s->RxBufPtr - s->RxBufAddr,
                 s->RxBufferSize);
    return avail == 0 || avail >= 1514 || (s->IntrMask & RxOverflow);
}