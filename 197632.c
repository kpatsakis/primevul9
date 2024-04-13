static int pcan_usb_fd_decode_buf(struct peak_usb_device *dev, struct urb *urb)
{
	struct pcan_usb_fd_if *usb_if = pcan_usb_fd_dev_if(dev);
	struct net_device *netdev = dev->netdev;
	struct pucan_msg *rx_msg;
	u8 *msg_ptr, *msg_end;
	int err = 0;

	/* loop reading all the records from the incoming message */
	msg_ptr = urb->transfer_buffer;
	msg_end = urb->transfer_buffer + urb->actual_length;
	for (; msg_ptr < msg_end;) {
		u16 rx_msg_type, rx_msg_size;

		rx_msg = (struct pucan_msg *)msg_ptr;
		if (!rx_msg->size) {
			/* null packet found: end of list */
			break;
		}

		rx_msg_size = le16_to_cpu(rx_msg->size);
		rx_msg_type = le16_to_cpu(rx_msg->type);

		/* check if the record goes out of current packet */
		if (msg_ptr + rx_msg_size > msg_end) {
			netdev_err(netdev,
				   "got frag rec: should inc usb rx buf sze\n");
			err = -EBADMSG;
			break;
		}

		switch (rx_msg_type) {
		case PUCAN_MSG_CAN_RX:
			err = pcan_usb_fd_decode_canmsg(usb_if, rx_msg);
			if (err < 0)
				goto fail;
			break;

		case PCAN_UFD_MSG_CALIBRATION:
			pcan_usb_fd_decode_ts(usb_if, rx_msg);
			break;

		case PUCAN_MSG_ERROR:
			err = pcan_usb_fd_decode_error(usb_if, rx_msg);
			if (err < 0)
				goto fail;
			break;

		case PUCAN_MSG_STATUS:
			err = pcan_usb_fd_decode_status(usb_if, rx_msg);
			if (err < 0)
				goto fail;
			break;

		case PCAN_UFD_MSG_OVERRUN:
			err = pcan_usb_fd_decode_overrun(usb_if, rx_msg);
			if (err < 0)
				goto fail;
			break;

		default:
			netdev_err(netdev,
				   "unhandled msg type 0x%02x (%d): ignored\n",
				   rx_msg_type, rx_msg_type);
			break;
		}

		msg_ptr += rx_msg_size;
	}

fail:
	if (err)
		pcan_dump_mem("received msg",
			      urb->transfer_buffer, urb->actual_length);
	return err;
}