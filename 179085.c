static int peak_usb_restart(struct peak_usb_device *dev)
{
	struct urb *urb;
	int err;
	u8 *buf;

	/*
	 * if device doesn't define any asynchronous restart handler, simply
	 * wake the netdev queue up
	 */
	if (!dev->adapter->dev_restart_async) {
		peak_usb_restart_complete(dev);
		return 0;
	}

	/* first allocate a urb to handle the asynchronous steps */
	urb = usb_alloc_urb(0, GFP_ATOMIC);
	if (!urb)
		return -ENOMEM;

	/* also allocate enough space for the commands to send */
	buf = kmalloc(PCAN_USB_MAX_CMD_LEN, GFP_ATOMIC);
	if (!buf) {
		usb_free_urb(urb);
		return -ENOMEM;
	}

	/* call the device specific handler for the restart */
	err = dev->adapter->dev_restart_async(dev, urb, buf);
	if (!err)
		return 0;

	kfree(buf);
	usb_free_urb(urb);

	return err;
}