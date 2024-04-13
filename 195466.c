static void ohci_set_frame_interval(OHCIState *ohci, uint16_t val)
{
    val &= OHCI_FMI_FI;

    if (val != ohci->fi) {
        trace_usb_ohci_set_frame_interval(ohci->name, ohci->fi, ohci->fi);
    }

    ohci->fi = val;
}