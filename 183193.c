static int pn533_usb_send_frame(struct pn533 *dev,
				struct sk_buff *out)
{
	struct pn533_usb_phy *phy = dev->phy;
	int rc;

	if (phy->priv == NULL)
		phy->priv = dev;

	phy->out_urb->transfer_buffer = out->data;
	phy->out_urb->transfer_buffer_length = out->len;

	print_hex_dump_debug("PN533 TX: ", DUMP_PREFIX_NONE, 16, 1,
			     out->data, out->len, false);

	rc = usb_submit_urb(phy->out_urb, GFP_KERNEL);
	if (rc)
		return rc;

	if (dev->protocol_type == PN533_PROTO_REQ_RESP) {
		/* request for response for sent packet directly */
		rc = pn533_submit_urb_for_response(phy, GFP_KERNEL);
		if (rc)
			goto error;
	} else if (dev->protocol_type == PN533_PROTO_REQ_ACK_RESP) {
		/* request for ACK if that's the case */
		rc = pn533_submit_urb_for_ack(phy, GFP_KERNEL);
		if (rc)
			goto error;
	}

	return 0;

error:
	usb_unlink_urb(phy->out_urb);
	return rc;
}