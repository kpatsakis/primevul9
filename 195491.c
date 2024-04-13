static void ohci_wakeup(USBPort *port1)
{
    OHCIState *s = port1->opaque;
    OHCIPort *port = &s->rhport[port1->index];
    uint32_t intr = 0;
    if (port->ctrl & OHCI_PORT_PSS) {
        trace_usb_ohci_port_wakeup(port1->index);
        port->ctrl |= OHCI_PORT_PSSC;
        port->ctrl &= ~OHCI_PORT_PSS;
        intr = OHCI_INTR_RHSC;
    }
    /* Note that the controller can be suspended even if this port is not */
    if ((s->ctl & OHCI_CTL_HCFS) == OHCI_USB_SUSPEND) {
        trace_usb_ohci_remote_wakeup(s->name);
        /* This is the one state transition the controller can do by itself */
        s->ctl &= ~OHCI_CTL_HCFS;
        s->ctl |= OHCI_USB_RESUME;
        /* In suspend mode only ResumeDetected is possible, not RHSC:
         * see the OHCI spec 5.1.2.3.
         */
        intr = OHCI_INTR_RD;
    }
    ohci_set_interrupt(s, intr);
}