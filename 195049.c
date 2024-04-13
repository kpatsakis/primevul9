static uint32_t rtl8139_CpCmd_read(RTL8139State *s)
{
    uint32_t ret = s->CpCmd;

    DPRINTF("C+ command register read(w) val=0x%04x\n", ret);

    return ret;
}