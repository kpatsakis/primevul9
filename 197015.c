static int ems_usb_open(struct net_device *netdev)
{
	struct ems_usb *dev = netdev_priv(netdev);
	int err;

	err = ems_usb_write_mode(dev, SJA1000_MOD_RM);
	if (err)
		return err;

	/* common open */
	err = open_candev(netdev);
	if (err)
		return err;

	/* finally start device */
	err = ems_usb_start(dev);
	if (err) {
		if (err == -ENODEV)
			netif_device_detach(dev->netdev);

		netdev_warn(netdev, "couldn't start device: %d\n", err);

		close_candev(netdev);

		return err;
	}


	netif_start_queue(netdev);

	return 0;
}