static void ohci_process_lists(OHCIState *ohci, int completion)
{
    if ((ohci->ctl & OHCI_CTL_CLE) && (ohci->status & OHCI_STATUS_CLF)) {
        if (ohci->ctrl_cur && ohci->ctrl_cur != ohci->ctrl_head) {
            trace_usb_ohci_process_lists(ohci->ctrl_head, ohci->ctrl_cur);
        }
        if (!ohci_service_ed_list(ohci, ohci->ctrl_head, completion)) {
            ohci->ctrl_cur = 0;
            ohci->status &= ~OHCI_STATUS_CLF;
        }
    }

    if ((ohci->ctl & OHCI_CTL_BLE) && (ohci->status & OHCI_STATUS_BLF)) {
        if (!ohci_service_ed_list(ohci, ohci->bulk_head, completion)) {
            ohci->bulk_cur = 0;
            ohci->status &= ~OHCI_STATUS_BLF;
        }
    }
}