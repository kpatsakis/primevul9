static void ohci_attach(USBPort *port1)
{
    OHCIState *s = port1->opaque;
    OHCIPort *port = &s->rhport[port1->index];
    uint32_t old_state = port->ctrl;

    /* set connect status */
    port->ctrl |= OHCI_PORT_CCS | OHCI_PORT_CSC;

    /* update speed */
    if (port->port.dev->speed == USB_SPEED_LOW) {
        port->ctrl |= OHCI_PORT_LSDA;
    } else {
        port->ctrl &= ~OHCI_PORT_LSDA;
    }

    /* notify of remote-wakeup */
    if ((s->ctl & OHCI_CTL_HCFS) == OHCI_USB_SUSPEND) {
        ohci_set_interrupt(s, OHCI_INTR_RD);
    }

    trace_usb_ohci_port_attach(port1->index);

    if (old_state != port->ctrl) {
        ohci_set_interrupt(s, OHCI_INTR_RHSC);
    }
}