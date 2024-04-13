static int ems_usb_close(struct net_device *netdev)
{
	struct ems_usb *dev = netdev_priv(netdev);

	/* Stop polling */
	unlink_all_urbs(dev);

	netif_stop_queue(netdev);

	/* Set CAN controller to reset mode */
	if (ems_usb_write_mode(dev, SJA1000_MOD_RM))
		netdev_warn(netdev, "couldn't stop device");

	close_candev(netdev);

	return 0;
}