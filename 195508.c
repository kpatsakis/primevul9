static USBDevice *ohci_find_device(OHCIState *ohci, uint8_t addr)
{
    USBDevice *dev;
    int i;

    for (i = 0; i < ohci->num_ports; i++) {
        if ((ohci->rhport[i].ctrl & OHCI_PORT_PES) == 0) {
            continue;
        }
        dev = usb_find_device(&ohci->rhport[i].port, addr);
        if (dev != NULL) {
            return dev;
        }
    }
    return NULL;
}