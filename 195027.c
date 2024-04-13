static uint32_t rtl8139_IntrMask_read(RTL8139State *s)
{
    uint32_t ret = s->IntrMask;

    DPRINTF("IntrMask read(w) val=0x%04x\n", ret);

    return ret;
}