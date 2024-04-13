static uint32_t rtl8139_TxAddr_read(RTL8139State *s, uint32_t txAddrOffset)
{
    uint32_t ret = s->TxAddr[txAddrOffset/4];

    DPRINTF("TxAddr read offset=0x%x val=0x%08x\n", txAddrOffset, ret);

    return ret;
}