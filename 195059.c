static uint32_t rtl8139_BasicModeStatus_read(RTL8139State *s)
{
    uint32_t ret = s->BasicModeStatus;

    DPRINTF("BasicModeStatus register read(w) val=0x%04x\n", ret);

    return ret;
}