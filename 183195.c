static int pn533_usb_send_ack(struct pn533 *dev, gfp_t flags)
{
	struct pn533_usb_phy *phy = dev->phy;
	static const u8 ack[6] = {0x00, 0x00, 0xff, 0x00, 0xff, 0x00};
	/* spec 7.1.1.3:  Preamble, SoPC (2), ACK Code (2), Postamble */

	if (!phy->ack_buffer) {
		phy->ack_buffer = kmemdup(ack, sizeof(ack), flags);
		if (!phy->ack_buffer)
			return -ENOMEM;
	}

	phy->ack_urb->transfer_buffer = phy->ack_buffer;
	phy->ack_urb->transfer_buffer_length = sizeof(ack);
	return usb_submit_urb(phy->ack_urb, flags);
}