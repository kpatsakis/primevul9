static void ems_usb_read_bulk_callback(struct urb *urb)
{
	struct ems_usb *dev = urb->context;
	struct net_device *netdev;
	int retval;

	netdev = dev->netdev;

	if (!netif_device_present(netdev))
		return;

	switch (urb->status) {
	case 0: /* success */
		break;

	case -ENOENT:
		return;

	default:
		netdev_info(netdev, "Rx URB aborted (%d)\n", urb->status);
		goto resubmit_urb;
	}

	if (urb->actual_length > CPC_HEADER_SIZE) {
		struct ems_cpc_msg *msg;
		u8 *ibuf = urb->transfer_buffer;
		u8 msg_count, start;

		msg_count = ibuf[0] & ~0x80;

		start = CPC_HEADER_SIZE;

		while (msg_count) {
			msg = (struct ems_cpc_msg *)&ibuf[start];

			switch (msg->type) {
			case CPC_MSG_TYPE_CAN_STATE:
				/* Process CAN state changes */
				ems_usb_rx_err(dev, msg);
				break;

			case CPC_MSG_TYPE_CAN_FRAME:
			case CPC_MSG_TYPE_EXT_CAN_FRAME:
			case CPC_MSG_TYPE_RTR_FRAME:
			case CPC_MSG_TYPE_EXT_RTR_FRAME:
				ems_usb_rx_can_msg(dev, msg);
				break;

			case CPC_MSG_TYPE_CAN_FRAME_ERROR:
				/* Process errorframe */
				ems_usb_rx_err(dev, msg);
				break;

			case CPC_MSG_TYPE_OVERRUN:
				/* Message lost while receiving */
				ems_usb_rx_err(dev, msg);
				break;
			}

			start += CPC_MSG_HEADER_LEN + msg->length;
			msg_count--;

			if (start > urb->transfer_buffer_length) {
				netdev_err(netdev, "format error\n");
				break;
			}
		}
	}

resubmit_urb:
	usb_fill_bulk_urb(urb, dev->udev, usb_rcvbulkpipe(dev->udev, 2),
			  urb->transfer_buffer, RX_BUFFER_SIZE,
			  ems_usb_read_bulk_callback, dev);

	retval = usb_submit_urb(urb, GFP_ATOMIC);

	if (retval == -ENODEV)
		netif_device_detach(netdev);
	else if (retval)
		netdev_err(netdev,
			   "failed resubmitting read bulk urb: %d\n", retval);
}