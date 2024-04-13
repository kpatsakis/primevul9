static int peak_usb_do_device_exit(struct device *d, void *arg)
{
	struct usb_interface *intf = to_usb_interface(d);
	struct peak_usb_device *dev;

	/* stop as many netdev devices as siblings */
	for (dev = usb_get_intfdata(intf); dev; dev = dev->prev_siblings) {
		struct net_device *netdev = dev->netdev;

		if (netif_device_present(netdev))
			if (dev->adapter->dev_exit)
				dev->adapter->dev_exit(dev);
	}

	return 0;
}