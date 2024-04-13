static void ohci_detach(USBPort *port1)
{
    OHCIState *s = port1->opaque;
    OHCIPort *port = &s->rhport[port1->index];
    uint32_t old_state = port->ctrl;

    ohci_async_cancel_device(s, port1->dev);

    /* set connect status */
    if (port->ctrl & OHCI_PORT_CCS) {
        port->ctrl &= ~OHCI_PORT_CCS;
        port->ctrl |= OHCI_PORT_CSC;
    }
    /* disable port */
    if (port->ctrl & OHCI_PORT_PES) {
        port->ctrl &= ~OHCI_PORT_PES;
        port->ctrl |= OHCI_PORT_PESC;
    }
    trace_usb_ohci_port_detach(port1->index);

    if (old_state != port->ctrl) {
        ohci_set_interrupt(s, OHCI_INTR_RHSC);
    }
}