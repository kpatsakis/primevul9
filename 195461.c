static void ohci_hard_reset(OHCIState *ohci)
{
    ohci_soft_reset(ohci);
    ohci->ctl = 0;
    ohci_roothub_reset(ohci);
}