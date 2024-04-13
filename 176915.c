static int pcan_usb_pro_set_led(struct peak_usb_device *dev, u8 mode,
				u32 timeout)
{
	struct pcan_usb_pro_msg um;

	pcan_msg_init_empty(&um, dev->cmd_buf, PCAN_USB_MAX_CMD_LEN);
	pcan_msg_add_rec(&um, PCAN_USBPRO_SETLED, dev->ctrl_idx, mode, timeout);

	return pcan_usb_pro_send_cmd(dev, &um);
}