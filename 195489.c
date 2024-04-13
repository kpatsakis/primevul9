static void ohci_sof(OHCIState *ohci)
{
    ohci_eof_timer(ohci);
    ohci_set_interrupt(ohci, OHCI_INTR_SF);
}