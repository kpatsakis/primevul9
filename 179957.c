static void usb_combined_packet_add(USBCombinedPacket *combined, USBPacket *p)
{
    qemu_iovec_concat(&combined->iov, &p->iov, 0, p->iov.size);
    QTAILQ_INSERT_TAIL(&combined->packets, p, combined_entry);
    p->combined = combined;
}