static void ohci_frame_boundary(void *opaque)
{
    OHCIState *ohci = opaque;
    struct ohci_hcca hcca;

    if (ohci_read_hcca(ohci, ohci->hcca, &hcca)) {
        trace_usb_ohci_hcca_read_error(ohci->hcca);
        ohci_die(ohci);
        return;
    }

    /* Process all the lists at the end of the frame */
    if (ohci->ctl & OHCI_CTL_PLE) {
        int n;

        n = ohci->frame_number & 0x1f;
        ohci_service_ed_list(ohci, le32_to_cpu(hcca.intr[n]), 0);
    }

    /* Cancel all pending packets if either of the lists has been disabled.  */
    if (ohci->old_ctl & (~ohci->ctl) & (OHCI_CTL_BLE | OHCI_CTL_CLE)) {
        if (ohci->async_td) {
            usb_cancel_packet(&ohci->usb_packet);
            ohci->async_td = 0;
        }
        ohci_stop_endpoints(ohci);
    }
    ohci->old_ctl = ohci->ctl;
    ohci_process_lists(ohci, 0);

    /* Stop if UnrecoverableError happened or ohci_sof will crash */
    if (ohci->intr_status & OHCI_INTR_UE) {
        return;
    }

    /* Frame boundary, so do EOF stuf here */
    ohci->frt = ohci->fit;

    /* Increment frame number and take care of endianness. */
    ohci->frame_number = (ohci->frame_number + 1) & 0xffff;
    hcca.frame = cpu_to_le16(ohci->frame_number);

    if (ohci->done_count == 0 && !(ohci->intr_status & OHCI_INTR_WD)) {
        if (!ohci->done)
            abort();
        if (ohci->intr & ohci->intr_status)
            ohci->done |= 1;
        hcca.done = cpu_to_le32(ohci->done);
        ohci->done = 0;
        ohci->done_count = 7;
        ohci_set_interrupt(ohci, OHCI_INTR_WD);
    }

    if (ohci->done_count != 7 && ohci->done_count != 0)
        ohci->done_count--;

    /* Do SOF stuff here */
    ohci_sof(ohci);

    /* Writeback HCCA */
    if (ohci_put_hcca(ohci, ohci->hcca, &hcca)) {
        ohci_die(ohci);
    }
}