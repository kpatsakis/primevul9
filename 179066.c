static void peak_usb_disconnect(struct usb_interface *intf)
{
	struct peak_usb_device *dev;
	struct peak_usb_device *dev_prev_siblings;

	/* unregister as many netdev devices as siblings */
	for (dev = usb_get_intfdata(intf); dev; dev = dev_prev_siblings) {
		struct net_device *netdev = dev->netdev;
		char name[IFNAMSIZ];

		dev_prev_siblings = dev->prev_siblings;
		dev->state &= ~PCAN_USB_STATE_CONNECTED;
		strlcpy(name, netdev->name, IFNAMSIZ);

		unregister_netdev(netdev);

		kfree(dev->cmd_buf);
		dev->next_siblings = NULL;
		if (dev->adapter->dev_free)
			dev->adapter->dev_free(dev);

		free_candev(netdev);
		dev_info(&intf->dev, "%s removed\n", name);
	}

	usb_set_intfdata(intf, NULL);
}