static void rtl8139_TxAddr_write(RTL8139State *s, uint32_t txAddrOffset, uint32_t val)
{
    DPRINTF("TxAddr write offset=0x%x val=0x%08x\n", txAddrOffset, val);

    s->TxAddr[txAddrOffset/4] = val;
}