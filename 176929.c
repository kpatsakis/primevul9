static int pcan_usb_pro_restart_async(struct peak_usb_device *dev,
				      struct urb *urb, u8 *buf)
{
	struct pcan_usb_pro_msg um;

	pcan_msg_init_empty(&um, buf, PCAN_USB_MAX_CMD_LEN);
	pcan_msg_add_rec(&um, PCAN_USBPRO_SETBUSACT, dev->ctrl_idx, 1);

	usb_fill_bulk_urb(urb, dev->udev,
			usb_sndbulkpipe(dev->udev, PCAN_USBPRO_EP_CMDOUT),
			buf, PCAN_USB_MAX_CMD_LEN,
			pcan_usb_pro_restart_complete, dev);

	return usb_submit_urb(urb, GFP_ATOMIC);
}