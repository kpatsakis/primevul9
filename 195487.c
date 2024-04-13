static void ohci_stop_endpoints(OHCIState *ohci)
{
    USBDevice *dev;
    int i, j;

    for (i = 0; i < ohci->num_ports; i++) {
        dev = ohci->rhport[i].port.dev;
        if (dev && dev->attached) {
            usb_device_ep_stopped(dev, &dev->ep_ctl);
            for (j = 0; j < USB_MAX_ENDPOINTS; j++) {
                usb_device_ep_stopped(dev, &dev->ep_in[j]);
                usb_device_ep_stopped(dev, &dev->ep_out[j]);
            }
        }
    }
}