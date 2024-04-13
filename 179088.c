static int peak_usb_set_bittiming(struct net_device *netdev)
{
	struct peak_usb_device *dev = netdev_priv(netdev);
	const struct peak_usb_adapter *pa = dev->adapter;

	if (pa->dev_set_bittiming) {
		struct can_bittiming *bt = &dev->can.bittiming;
		int err = pa->dev_set_bittiming(dev, bt);

		if (err)
			netdev_info(netdev, "couldn't set bitrate (err %d)\n",
				    err);
		return err;
	}

	return 0;
}