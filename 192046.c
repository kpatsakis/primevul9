u8 rtl8xxxu_read8(struct rtl8xxxu_priv *priv, u16 addr)
{
	struct usb_device *udev = priv->udev;
	int len;
	u8 data;

	mutex_lock(&priv->usb_buf_mutex);
	len = usb_control_msg(udev, usb_rcvctrlpipe(udev, 0),
			      REALTEK_USB_CMD_REQ, REALTEK_USB_READ,
			      addr, 0, &priv->usb_buf.val8, sizeof(u8),
			      RTW_USB_CONTROL_MSG_TIMEOUT);
	data = priv->usb_buf.val8;
	mutex_unlock(&priv->usb_buf_mutex);

	if (rtl8xxxu_debug & RTL8XXXU_DEBUG_REG_READ)
		dev_info(&udev->dev, "%s(%04x)   = 0x%02x, len %i\n",
			 __func__, addr, data, len);
	return data;
}