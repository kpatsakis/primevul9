static int peak_usb_set_mode(struct net_device *netdev, enum can_mode mode)
{
	struct peak_usb_device *dev = netdev_priv(netdev);
	int err = 0;

	switch (mode) {
	case CAN_MODE_START:
		err = peak_usb_restart(dev);
		if (err)
			netdev_err(netdev, "couldn't start device (err %d)\n",
				   err);
		break;

	default:
		return -EOPNOTSUPP;
	}

	return err;
}