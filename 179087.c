static int peak_usb_ndo_open(struct net_device *netdev)
{
	struct peak_usb_device *dev = netdev_priv(netdev);
	int err;

	/* common open */
	err = open_candev(netdev);
	if (err)
		return err;

	/* finally start device */
	err = peak_usb_start(dev);
	if (err) {
		netdev_err(netdev, "couldn't start device: %d\n", err);
		close_candev(netdev);
		return err;
	}

	netif_start_queue(netdev);

	return 0;
}