static void RTL8139TallyCounters_clear(RTL8139TallyCounters* counters)
{
    counters->TxOk = 0;
    counters->RxOk = 0;
    counters->TxERR = 0;
    counters->RxERR = 0;
    counters->MissPkt = 0;
    counters->FAE = 0;
    counters->Tx1Col = 0;
    counters->TxMCol = 0;
    counters->RxOkPhy = 0;
    counters->RxOkBrd = 0;
    counters->RxOkMul = 0;
    counters->TxAbt = 0;
    counters->TxUndrn = 0;
}