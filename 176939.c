static int pcan_usb_pro_set_bitrate(struct peak_usb_device *dev, u32 ccbt)
{
	struct pcan_usb_pro_device *pdev =
			container_of(dev, struct pcan_usb_pro_device, dev);
	struct pcan_usb_pro_msg um;

	pcan_msg_init_empty(&um, dev->cmd_buf, PCAN_USB_MAX_CMD_LEN);
	pcan_msg_add_rec(&um, PCAN_USBPRO_SETBTR, dev->ctrl_idx, ccbt);

	/* cache the CCBT value to reuse it before next buson */
	pdev->cached_ccbt = ccbt;

	return pcan_usb_pro_send_cmd(dev, &um);
}