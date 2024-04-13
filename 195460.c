static void ohci_child_detach(USBPort *port1, USBDevice *child)
{
    OHCIState *s = port1->opaque;

    ohci_async_cancel_device(s, child);
}