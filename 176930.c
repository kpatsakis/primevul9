int pcan_usb_pro_send_req(struct peak_usb_device *dev, int req_id,
			  int req_value, void *req_addr, int req_size)
{
	int err;
	u8 req_type;
	unsigned int p;

	/* usb device unregistered? */
	if (!(dev->state & PCAN_USB_STATE_CONNECTED))
		return 0;

	req_type = USB_TYPE_VENDOR | USB_RECIP_OTHER;

	switch (req_id) {
	case PCAN_USBPRO_REQ_FCT:
		p = usb_sndctrlpipe(dev->udev, 0);
		break;

	default:
		p = usb_rcvctrlpipe(dev->udev, 0);
		req_type |= USB_DIR_IN;
		memset(req_addr, '\0', req_size);
		break;
	}

	err = usb_control_msg(dev->udev, p, req_id, req_type, req_value, 0,
			      req_addr, req_size, 2 * USB_CTRL_GET_TIMEOUT);
	if (err < 0) {
		netdev_info(dev->netdev,
			    "unable to request usb[type=%d value=%d] err=%d\n",
			    req_id, req_value, err);
		return err;
	}

	return 0;
}