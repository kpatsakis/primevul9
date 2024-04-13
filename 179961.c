void usb_combined_packet_cancel(USBDevice *dev, USBPacket *p)
{
    USBCombinedPacket *combined = p->combined;
    assert(combined != NULL);
    USBPacket *first = p->combined->first;

    /* Note will free combined on the last packet! */
    usb_combined_packet_remove(combined, p);
    if (p == first) {
        usb_device_cancel_packet(dev, p);
    }
}