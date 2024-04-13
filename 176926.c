static int pcan_usb_pro_set_bus(struct peak_usb_device *dev, u8 onoff)
{
	struct pcan_usb_pro_msg um;

	/* if bus=on, be sure the bitrate being set before! */
	if (onoff) {
		struct pcan_usb_pro_device *pdev =
			     container_of(dev, struct pcan_usb_pro_device, dev);

		pcan_usb_pro_set_bitrate(dev, pdev->cached_ccbt);
	}

	pcan_msg_init_empty(&um, dev->cmd_buf, PCAN_USB_MAX_CMD_LEN);
	pcan_msg_add_rec(&um, PCAN_USBPRO_SETBUSACT, dev->ctrl_idx, onoff);

	return pcan_usb_pro_send_cmd(dev, &um);
}