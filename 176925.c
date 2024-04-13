static int pcan_usb_pro_decode_buf(struct peak_usb_device *dev, struct urb *urb)
{
	struct pcan_usb_pro_interface *usb_if = pcan_usb_pro_dev_if(dev);
	struct net_device *netdev = dev->netdev;
	struct pcan_usb_pro_msg usb_msg;
	u8 *rec_ptr, *msg_end;
	u16 rec_cnt;
	int err = 0;

	rec_ptr = pcan_msg_init(&usb_msg, urb->transfer_buffer,
					urb->actual_length);
	if (!rec_ptr) {
		netdev_err(netdev, "bad msg hdr len %d\n", urb->actual_length);
		return -EINVAL;
	}

	/* loop reading all the records from the incoming message */
	msg_end = urb->transfer_buffer + urb->actual_length;
	rec_cnt = le16_to_cpu(*usb_msg.u.rec_cnt_rd);
	for (; rec_cnt > 0; rec_cnt--) {
		union pcan_usb_pro_rec *pr = (union pcan_usb_pro_rec *)rec_ptr;
		u16 sizeof_rec = pcan_usb_pro_sizeof_rec[pr->data_type];

		if (!sizeof_rec) {
			netdev_err(netdev,
				   "got unsupported rec in usb msg:\n");
			err = -ENOTSUPP;
			break;
		}

		/* check if the record goes out of current packet */
		if (rec_ptr + sizeof_rec > msg_end) {
			netdev_err(netdev,
				"got frag rec: should inc usb rx buf size\n");
			err = -EBADMSG;
			break;
		}

		switch (pr->data_type) {
		case PCAN_USBPRO_RXMSG8:
		case PCAN_USBPRO_RXMSG4:
		case PCAN_USBPRO_RXMSG0:
		case PCAN_USBPRO_RXRTR:
			err = pcan_usb_pro_handle_canmsg(usb_if, &pr->rx_msg);
			if (err < 0)
				goto fail;
			break;

		case PCAN_USBPRO_RXSTATUS:
			err = pcan_usb_pro_handle_error(usb_if, &pr->rx_status);
			if (err < 0)
				goto fail;
			break;

		case PCAN_USBPRO_RXTS:
			pcan_usb_pro_handle_ts(usb_if, &pr->rx_ts);
			break;

		default:
			netdev_err(netdev,
				   "unhandled rec type 0x%02x (%d): ignored\n",
				   pr->data_type, pr->data_type);
			break;
		}

		rec_ptr += sizeof_rec;
	}

fail:
	if (err)
		pcan_dump_mem("received msg",
			      urb->transfer_buffer, urb->actual_length);

	return err;
}