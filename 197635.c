static int pcan_usb_fd_get_berr_counter(const struct net_device *netdev,
					struct can_berr_counter *bec)
{
	struct peak_usb_device *dev = netdev_priv(netdev);
	struct pcan_usb_fd_device *pdev =
			container_of(dev, struct pcan_usb_fd_device, dev);

	*bec = pdev->bec;

	/* must return 0 */
	return 0;
}