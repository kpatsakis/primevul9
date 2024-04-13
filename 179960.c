void usb_combined_input_packet_complete(USBDevice *dev, USBPacket *p)
{
    USBCombinedPacket *combined = p->combined;
    USBEndpoint *ep = p->ep;
    USBPacket *next;
    int status, actual_length;
    bool short_not_ok, done = false;

    if (combined == NULL) {
        usb_packet_complete_one(dev, p);
        goto leave;
    }

    assert(combined->first == p && p == QTAILQ_FIRST(&combined->packets));

    status = combined->first->status;
    actual_length = combined->first->actual_length;
    short_not_ok = QTAILQ_LAST(&combined->packets)->short_not_ok;

    QTAILQ_FOREACH_SAFE(p, &combined->packets, combined_entry, next) {
        if (!done) {
            /* Distribute data over uncombined packets */
            if (actual_length >= p->iov.size) {
                p->actual_length = p->iov.size;
            } else {
                /* Send short or error packet to complete the transfer */
                p->actual_length = actual_length;
                done = true;
            }
            /* Report status on the last packet */
            if (done || next == NULL) {
                p->status = status;
            } else {
                p->status = USB_RET_SUCCESS;
            }
            p->short_not_ok = short_not_ok;
            /* Note will free combined when the last packet gets removed! */
            usb_combined_packet_remove(combined, p);
            usb_packet_complete_one(dev, p);
            actual_length -= p->actual_length;
        } else {
            /* Remove any leftover packets from the queue */
            p->status = USB_RET_REMOVE_FROM_QUEUE;
            /* Note will free combined on the last packet! */
            dev->port->ops->complete(dev->port, p);
        }
    }
    /* Do not use combined here, it has been freed! */
leave:
    /* Check if there are packets in the queue waiting for our completion */
    usb_ep_combine_input_packets(ep);
}