static void usb_ohci_reset_sysbus(DeviceState *dev)
{
    OHCISysBusState *s = SYSBUS_OHCI(dev);
    OHCIState *ohci = &s->ohci;

    ohci_hard_reset(ohci);
}