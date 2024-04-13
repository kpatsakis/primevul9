u32 rtl8xxxu_read32(struct rtl8xxxu_priv *priv, u16 addr)
{
	struct usb_device *udev = priv->udev;
	int len;
	u32 data;

	mutex_lock(&priv->usb_buf_mutex);
	len = usb_control_msg(udev, usb_rcvctrlpipe(udev, 0),
			      REALTEK_USB_CMD_REQ, REALTEK_USB_READ,
			      addr, 0, &priv->usb_buf.val32, sizeof(u32),
			      RTW_USB_CONTROL_MSG_TIMEOUT);
	data = le32_to_cpu(priv->usb_buf.val32);
	mutex_unlock(&priv->usb_buf_mutex);

	if (rtl8xxxu_debug & RTL8XXXU_DEBUG_REG_READ)
		dev_info(&udev->dev, "%s(%04x)  = 0x%08x, len %i\n",
			 __func__, addr, data, len);
	return data;
}