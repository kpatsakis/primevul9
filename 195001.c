static void rtl8139_reset_phy(RTL8139State *s)
{
    s->BasicModeStatus  = 0x7809;
    s->BasicModeStatus |= 0x0020; /* autonegotiation completed */
    /* preserve link state */
    s->BasicModeStatus |= qemu_get_queue(s->nic)->link_down ? 0 : 0x04;

    s->NWayAdvert    = 0x05e1; /* all modes, full duplex */
    s->NWayLPAR      = 0x05e1; /* all modes, full duplex */
    s->NWayExpansion = 0x0001; /* autonegotiation supported */

    s->CSCR = CSCR_F_LINK_100 | CSCR_HEART_BIT | CSCR_LD;
}