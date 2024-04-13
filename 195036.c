static uint32_t rtl8139_MultiIntr_read(RTL8139State *s)
{
    uint32_t ret = s->MultiIntr;

    DPRINTF("MultiIntr read(w) val=0x%04x\n", ret);

    return ret;
}