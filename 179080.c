void peak_usb_restart_complete(struct peak_usb_device *dev)
{
	/* finally MUST update can state */
	dev->can.state = CAN_STATE_ERROR_ACTIVE;

	/* netdev queue can be awaken now */
	netif_wake_queue(dev->netdev);
}