static int peak_usb_create_dev(const struct peak_usb_adapter *peak_usb_adapter,
			       struct usb_interface *intf, int ctrl_idx)
{
	struct usb_device *usb_dev = interface_to_usbdev(intf);
	int sizeof_candev = peak_usb_adapter->sizeof_dev_private;
	struct peak_usb_device *dev;
	struct net_device *netdev;
	int i, err;
	u16 tmp16;

	if (sizeof_candev < sizeof(struct peak_usb_device))
		sizeof_candev = sizeof(struct peak_usb_device);

	netdev = alloc_candev(sizeof_candev, PCAN_USB_MAX_TX_URBS);
	if (!netdev) {
		dev_err(&intf->dev, "%s: couldn't alloc candev\n",
			PCAN_USB_DRIVER_NAME);
		return -ENOMEM;
	}

	dev = netdev_priv(netdev);

	/* allocate a buffer large enough to send commands */
	dev->cmd_buf = kzalloc(PCAN_USB_MAX_CMD_LEN, GFP_KERNEL);
	if (!dev->cmd_buf) {
		err = -ENOMEM;
		goto lbl_free_candev;
	}

	dev->udev = usb_dev;
	dev->netdev = netdev;
	dev->adapter = peak_usb_adapter;
	dev->ctrl_idx = ctrl_idx;
	dev->state = PCAN_USB_STATE_CONNECTED;

	dev->ep_msg_in = peak_usb_adapter->ep_msg_in;
	dev->ep_msg_out = peak_usb_adapter->ep_msg_out[ctrl_idx];

	dev->can.clock = peak_usb_adapter->clock;
	dev->can.bittiming_const = peak_usb_adapter->bittiming_const;
	dev->can.do_set_bittiming = peak_usb_set_bittiming;
	dev->can.data_bittiming_const = peak_usb_adapter->data_bittiming_const;
	dev->can.do_set_data_bittiming = peak_usb_set_data_bittiming;
	dev->can.do_set_mode = peak_usb_set_mode;
	dev->can.do_get_berr_counter = peak_usb_adapter->do_get_berr_counter;
	dev->can.ctrlmode_supported = peak_usb_adapter->ctrlmode_supported;

	netdev->netdev_ops = &peak_usb_netdev_ops;

	netdev->flags |= IFF_ECHO; /* we support local echo */

	init_usb_anchor(&dev->rx_submitted);

	init_usb_anchor(&dev->tx_submitted);
	atomic_set(&dev->active_tx_urbs, 0);

	for (i = 0; i < PCAN_USB_MAX_TX_URBS; i++)
		dev->tx_contexts[i].echo_index = PCAN_USB_MAX_TX_URBS;

	dev->prev_siblings = usb_get_intfdata(intf);
	usb_set_intfdata(intf, dev);

	SET_NETDEV_DEV(netdev, &intf->dev);
	netdev->dev_id = ctrl_idx;

	err = register_candev(netdev);
	if (err) {
		dev_err(&intf->dev, "couldn't register CAN device: %d\n", err);
		goto lbl_restore_intf_data;
	}

	if (dev->prev_siblings)
		(dev->prev_siblings)->next_siblings = dev;

	/* keep hw revision into the netdevice */
	tmp16 = le16_to_cpu(usb_dev->descriptor.bcdDevice);
	dev->device_rev = tmp16 >> 8;

	if (dev->adapter->dev_init) {
		err = dev->adapter->dev_init(dev);
		if (err)
			goto lbl_unregister_candev;
	}

	/* set bus off */
	if (dev->adapter->dev_set_bus) {
		err = dev->adapter->dev_set_bus(dev, 0);
		if (err)
			goto lbl_unregister_candev;
	}

	/* get device number early */
	if (dev->adapter->dev_get_device_id)
		dev->adapter->dev_get_device_id(dev, &dev->device_number);

	netdev_info(netdev, "attached to %s channel %u (device %u)\n",
			peak_usb_adapter->name, ctrl_idx, dev->device_number);

	return 0;

lbl_unregister_candev:
	unregister_candev(netdev);

lbl_restore_intf_data:
	usb_set_intfdata(intf, dev->prev_siblings);
	kfree(dev->cmd_buf);

lbl_free_candev:
	free_candev(netdev);

	return err;
}