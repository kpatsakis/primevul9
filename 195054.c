static void rtl8139_BasicModeCtrl_write(RTL8139State *s, uint32_t val)
{
    val &= 0xffff;

    DPRINTF("BasicModeCtrl register write(w) val=0x%04x\n", val);

    /* mask unwritable bits */
    uint32_t mask = 0xccff;

    if (1 || !rtl8139_config_writable(s))
    {
        /* Speed setting and autonegotiation enable bits are read-only */
        mask |= 0x3000;
        /* Duplex mode setting is read-only */
        mask |= 0x0100;
    }

    if (val & 0x8000) {
        /* Reset PHY */
        rtl8139_reset_phy(s);
    }

    val = SET_MASKED(val, mask, s->BasicModeCtrl);

    s->BasicModeCtrl = val;
}