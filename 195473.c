static void ohci_port_power(OHCIState *ohci, int i, int p)
{
    if (p) {
        ohci->rhport[i].ctrl |= OHCI_PORT_PPS;
    } else {
        ohci->rhport[i].ctrl &= ~(OHCI_PORT_PPS|
                    OHCI_PORT_CCS|
                    OHCI_PORT_PSS|
                    OHCI_PORT_PRS);
    }
}