static void rtl8139_TxConfig_writeb(RTL8139State *s, uint32_t val)
{
    DPRINTF("RTL8139C TxConfig via write(b) val=0x%02x\n", val);

    uint32_t tc = s->TxConfig;
    tc &= 0xFFFFFF00;
    tc |= (val & 0x000000FF);
    rtl8139_TxConfig_write(s, tc);
}