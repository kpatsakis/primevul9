static void usb_combined_packet_remove(USBCombinedPacket *combined,
                                       USBPacket *p)
{
    assert(p->combined == combined);
    p->combined = NULL;
    QTAILQ_REMOVE(&combined->packets, p, combined_entry);
    if (QTAILQ_EMPTY(&combined->packets)) {
        qemu_iovec_destroy(&combined->iov);
        g_free(combined);
    }
}