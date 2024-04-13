static int ems_usb_set_mode(struct net_device *netdev, enum can_mode mode)
{
	struct ems_usb *dev = netdev_priv(netdev);

	switch (mode) {
	case CAN_MODE_START:
		if (ems_usb_write_mode(dev, SJA1000_MOD_NORMAL))
			netdev_warn(netdev, "couldn't start device");

		if (netif_queue_stopped(netdev))
			netif_wake_queue(netdev);
		break;

	default:
		return -EOPNOTSUPP;
	}

	return 0;
}