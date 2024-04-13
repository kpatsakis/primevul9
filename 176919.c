static int pcan_usb_pro_send_cmd(struct peak_usb_device *dev,
				 struct pcan_usb_pro_msg *pum)
{
	int actual_length;
	int err;

	/* usb device unregistered? */
	if (!(dev->state & PCAN_USB_STATE_CONNECTED))
		return 0;

	err = usb_bulk_msg(dev->udev,
		usb_sndbulkpipe(dev->udev, PCAN_USBPRO_EP_CMDOUT),
		pum->u.rec_buffer, pum->rec_buffer_len,
		&actual_length, PCAN_USBPRO_COMMAND_TIMEOUT);
	if (err)
		netdev_err(dev->netdev, "sending command failure: %d\n", err);

	return err;
}