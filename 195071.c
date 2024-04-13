static uint32_t rtl8139_BasicModeCtrl_read(RTL8139State *s)
{
    uint32_t ret = s->BasicModeCtrl;

    DPRINTF("BasicModeCtrl register read(w) val=0x%04x\n", ret);

    return ret;
}