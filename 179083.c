static int peak_usb_ndo_stop(struct net_device *netdev)
{
	struct peak_usb_device *dev = netdev_priv(netdev);

	dev->state &= ~PCAN_USB_STATE_STARTED;
	netif_stop_queue(netdev);

	close_candev(netdev);

	dev->can.state = CAN_STATE_STOPPED;

	/* unlink all pending urbs and free used memory */
	peak_usb_unlink_all_urbs(dev);

	if (dev->adapter->dev_stop)
		dev->adapter->dev_stop(dev);

	/* can set bus off now */
	if (dev->adapter->dev_set_bus) {
		int err = dev->adapter->dev_set_bus(dev, 0);
		if (err)
			return err;
	}

	return 0;
}