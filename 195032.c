static void rtl8139_reset(DeviceState *d)
{
    RTL8139State *s = RTL8139(d);
    int i;

    /* restore MAC address */
    memcpy(s->phys, s->conf.macaddr.a, 6);
    qemu_format_nic_info_str(qemu_get_queue(s->nic), s->phys);

    /* reset interrupt mask */
    s->IntrStatus = 0;
    s->IntrMask = 0;

    rtl8139_update_irq(s);

    /* mark all status registers as owned by host */
    for (i = 0; i < 4; ++i)
    {
        s->TxStatus[i] = TxHostOwns;
    }

    s->currTxDesc = 0;
    s->currCPlusRxDesc = 0;
    s->currCPlusTxDesc = 0;

    s->RxRingAddrLO = 0;
    s->RxRingAddrHI = 0;

    s->RxBuf = 0;

    rtl8139_reset_rxring(s, 8192);

    /* ACK the reset */
    s->TxConfig = 0;

#if 0
//    s->TxConfig |= HW_REVID(1, 0, 0, 0, 0, 0, 0); // RTL-8139  HasHltClk
    s->clock_enabled = 0;
#else
    s->TxConfig |= HW_REVID(1, 1, 1, 0, 1, 1, 0); // RTL-8139C+ HasLWake
    s->clock_enabled = 1;
#endif

    s->bChipCmdState = CmdReset; /* RxBufEmpty bit is calculated on read from ChipCmd */;

    /* set initial state data */
    s->Config0 = 0x0; /* No boot ROM */
    s->Config1 = 0xC; /* IO mapped and MEM mapped registers available */
    s->Config3 = 0x1; /* fast back-to-back compatible */
    s->Config5 = 0x0;

    s->CpCmd   = 0x0; /* reset C+ mode */
    s->cplus_enabled = 0;

//    s->BasicModeCtrl = 0x3100; // 100Mbps, full duplex, autonegotiation
//    s->BasicModeCtrl = 0x2100; // 100Mbps, full duplex
    s->BasicModeCtrl = 0x1000; // autonegotiation

    rtl8139_reset_phy(s);

    /* also reset timer and disable timer interrupt */
    s->TCTR = 0;
    s->TimerInt = 0;
    s->TCTR_base = 0;
    rtl8139_set_next_tctr_time(s);

    /* reset tally counters */
    RTL8139TallyCounters_clear(&s->tally_counters);
}