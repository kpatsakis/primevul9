static void rtl8139_TxConfig_write(RTL8139State *s, uint32_t val)
{
    if (!rtl8139_transmitter_enabled(s))
    {
        DPRINTF("transmitter disabled; no TxConfig write val=0x%08x\n", val);
        return;
    }

    DPRINTF("TxConfig write val=0x%08x\n", val);

    val = SET_MASKED(val, TxVersionMask | 0x8070f80f, s->TxConfig);

    s->TxConfig = val;
}