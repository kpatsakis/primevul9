static void rtl8139_TxStatus_write(RTL8139State *s, uint32_t txRegOffset, uint32_t val)
{

    int descriptor = txRegOffset/4;

    /* handle C+ transmit mode register configuration */

    if (s->cplus_enabled)
    {
        DPRINTF("RTL8139C+ DTCCR write offset=0x%x val=0x%08x "
            "descriptor=%d\n", txRegOffset, val, descriptor);

        /* handle Dump Tally Counters command */
        s->TxStatus[descriptor] = val;

        if (descriptor == 0 && (val & 0x8))
        {
            hwaddr tc_addr = rtl8139_addr64(s->TxStatus[0] & ~0x3f, s->TxStatus[1]);

            /* dump tally counters to specified memory location */
            RTL8139TallyCounters_dma_write(s, tc_addr);

            /* mark dump completed */
            s->TxStatus[0] &= ~0x8;
        }

        return;
    }

    DPRINTF("TxStatus write offset=0x%x val=0x%08x descriptor=%d\n",
        txRegOffset, val, descriptor);

    /* mask only reserved bits */
    val &= ~0xff00c000; /* these bits are reset on write */
    val = SET_MASKED(val, 0x00c00000, s->TxStatus[descriptor]);

    s->TxStatus[descriptor] = val;

    /* attempt to start transmission */
    rtl8139_transmit(s);
}