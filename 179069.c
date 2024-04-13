static int peak_usb_set_data_bittiming(struct net_device *netdev)
{
	struct peak_usb_device *dev = netdev_priv(netdev);
	const struct peak_usb_adapter *pa = dev->adapter;

	if (pa->dev_set_data_bittiming) {
		struct can_bittiming *bt = &dev->can.data_bittiming;
		int err = pa->dev_set_data_bittiming(dev, bt);

		if (err)
			netdev_info(netdev,
				    "couldn't set data bitrate (err %d)\n",
				    err);

		return err;
	}

	return 0;
}