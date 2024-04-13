static void ohci_async_complete_packet(USBPort *port, USBPacket *packet)
{
    OHCIState *ohci = container_of(packet, OHCIState, usb_packet);

    trace_usb_ohci_async_complete();
    ohci->async_complete = true;
    ohci_process_lists(ohci, 1);
}