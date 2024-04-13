static uint32_t ohci_get_frame_remaining(OHCIState *ohci)
{
    uint16_t fr;
    int64_t tks;

    if ((ohci->ctl & OHCI_CTL_HCFS) != OHCI_USB_OPERATIONAL)
        return (ohci->frt << 31);

    /* Being in USB operational state guarnatees sof_time was
     * set already.
     */
    tks = qemu_clock_get_ns(QEMU_CLOCK_VIRTUAL) - ohci->sof_time;

    /* avoid muldiv if possible */
    if (tks >= usb_frame_time)
        return (ohci->frt << 31);

    tks = tks / usb_bit_time;
    fr = (uint16_t)(ohci->fi - tks);

    return (ohci->frt << 31) | fr;
}