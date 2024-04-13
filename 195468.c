static void ohci_set_ctl(OHCIState *ohci, uint32_t val)
{
    uint32_t old_state;
    uint32_t new_state;

    old_state = ohci->ctl & OHCI_CTL_HCFS;
    ohci->ctl = val;
    new_state = ohci->ctl & OHCI_CTL_HCFS;

    /* no state change */
    if (old_state == new_state)
        return;

    trace_usb_ohci_set_ctl(ohci->name, new_state);
    switch (new_state) {
    case OHCI_USB_OPERATIONAL:
        ohci_bus_start(ohci);
        break;
    case OHCI_USB_SUSPEND:
        ohci_bus_stop(ohci);
        /* clear pending SF otherwise linux driver loops in ohci_irq() */
        ohci->intr_status &= ~OHCI_INTR_SF;
        ohci_intr_update(ohci);
        break;
    case OHCI_USB_RESUME:
        trace_usb_ohci_resume(ohci->name);
        break;
    case OHCI_USB_RESET:
        ohci_roothub_reset(ohci);
        break;
    }
}