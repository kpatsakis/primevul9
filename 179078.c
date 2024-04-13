static int peak_usb_probe(struct usb_interface *intf,
			  const struct usb_device_id *id)
{
	struct usb_device *usb_dev = interface_to_usbdev(intf);
	const u16 usb_id_product = le16_to_cpu(usb_dev->descriptor.idProduct);
	const struct peak_usb_adapter *peak_usb_adapter = NULL;
	int i, err = -ENOMEM;

	/* get corresponding PCAN-USB adapter */
	for (i = 0; i < ARRAY_SIZE(peak_usb_adapters_list); i++)
		if (peak_usb_adapters_list[i]->device_id == usb_id_product) {
			peak_usb_adapter = peak_usb_adapters_list[i];
			break;
		}

	if (!peak_usb_adapter) {
		/* should never come except device_id bad usage in this file */
		pr_err("%s: didn't find device id. 0x%x in devices list\n",
			PCAN_USB_DRIVER_NAME, usb_id_product);
		return -ENODEV;
	}

	/* got corresponding adapter: check if it handles current interface */
	if (peak_usb_adapter->intf_probe) {
		err = peak_usb_adapter->intf_probe(intf);
		if (err)
			return err;
	}

	for (i = 0; i < peak_usb_adapter->ctrl_count; i++) {
		err = peak_usb_create_dev(peak_usb_adapter, intf, i);
		if (err) {
			/* deregister already created devices */
			peak_usb_disconnect(intf);
			break;
		}
	}

	return err;
}