int rtl8xxxu_write16(struct rtl8xxxu_priv *priv, u16 addr, u16 val)
{
	struct usb_device *udev = priv->udev;
	int ret;

	mutex_lock(&priv->usb_buf_mutex);
	priv->usb_buf.val16 = cpu_to_le16(val);
	ret = usb_control_msg(udev, usb_sndctrlpipe(udev, 0),
			      REALTEK_USB_CMD_REQ, REALTEK_USB_WRITE,
			      addr, 0, &priv->usb_buf.val16, sizeof(u16),
			      RTW_USB_CONTROL_MSG_TIMEOUT);
	mutex_unlock(&priv->usb_buf_mutex);

	if (rtl8xxxu_debug & RTL8XXXU_DEBUG_REG_WRITE)
		dev_info(&udev->dev, "%s(%04x) = 0x%04x\n",
			 __func__, addr, val);
	return ret;
}