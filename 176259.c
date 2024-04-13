static void ax88179_write_cmd_async(struct usbnet *dev, u8 cmd, u16 value,
				    u16 index, u16 size, void *data)
{
	u16 buf;

	if (2 == size) {
		buf = *((u16 *)data);
		cpu_to_le16s(&buf);
		usbnet_write_cmd_async(dev, cmd, USB_DIR_OUT | USB_TYPE_VENDOR |
				       USB_RECIP_DEVICE, value, index, &buf,
				       size);
	} else {
		usbnet_write_cmd_async(dev, cmd, USB_DIR_OUT | USB_TYPE_VENDOR |
				       USB_RECIP_DEVICE, value, index, data,
				       size);
	}
}