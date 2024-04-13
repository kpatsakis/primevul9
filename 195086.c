static uint16_t rtl8139_CSCR_read(RTL8139State *s)
{
    uint16_t ret = s->CSCR;

    DPRINTF("CSCR read val=0x%04x\n", ret);

    return ret;
}