rtl8xxxu_writeN(struct rtl8xxxu_priv *priv, u16 addr, u8 *buf, u16 len)
{
	struct usb_device *udev = priv->udev;
	int blocksize = priv->fops->writeN_block_size;
	int ret, i, count, remainder;

	count = len / blocksize;
	remainder = len % blocksize;

	for (i = 0; i < count; i++) {
		ret = usb_control_msg(udev, usb_sndctrlpipe(udev, 0),
				      REALTEK_USB_CMD_REQ, REALTEK_USB_WRITE,
				      addr, 0, buf, blocksize,
				      RTW_USB_CONTROL_MSG_TIMEOUT);
		if (ret != blocksize)
			goto write_error;

		addr += blocksize;
		buf += blocksize;
	}

	if (remainder) {
		ret = usb_control_msg(udev, usb_sndctrlpipe(udev, 0),
				      REALTEK_USB_CMD_REQ, REALTEK_USB_WRITE,
				      addr, 0, buf, remainder,
				      RTW_USB_CONTROL_MSG_TIMEOUT);
		if (ret != remainder)
			goto write_error;
	}

	return len;

write_error:
	dev_info(&udev->dev,
		 "%s: Failed to write block at addr: %04x size: %04x\n",
		 __func__, addr, blocksize);
	return -EAGAIN;
}