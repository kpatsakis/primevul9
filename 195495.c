static void ohci_roothub_reset(OHCIState *ohci)
{
    OHCIPort *port;
    int i;

    ohci_bus_stop(ohci);
    ohci->rhdesc_a = OHCI_RHA_NPS | ohci->num_ports;
    ohci->rhdesc_b = 0x0; /* Impl. specific */
    ohci->rhstatus = 0;

    for (i = 0; i < ohci->num_ports; i++) {
        port = &ohci->rhport[i];
        port->ctrl = 0;
        if (port->port.dev && port->port.dev->attached) {
            usb_port_reset(&port->port);
        }
    }
    if (ohci->async_td) {
        usb_cancel_packet(&ohci->usb_packet);
        ohci->async_td = 0;
    }
    ohci_stop_endpoints(ohci);
}