static void ems_usb_read_interrupt_callback(struct urb *urb)
{
	struct ems_usb *dev = urb->context;
	struct net_device *netdev = dev->netdev;
	int err;

	if (!netif_device_present(netdev))
		return;

	switch (urb->status) {
	case 0:
		dev->free_slots = dev->intr_in_buffer[1];
		if (dev->free_slots > CPC_TX_QUEUE_TRIGGER_HIGH &&
		    netif_queue_stopped(netdev))
			netif_wake_queue(netdev);
		break;

	case -ECONNRESET: /* unlink */
	case -ENOENT:
	case -EPIPE:
	case -EPROTO:
	case -ESHUTDOWN:
		return;

	default:
		netdev_info(netdev, "Rx interrupt aborted %d\n", urb->status);
		break;
	}

	err = usb_submit_urb(urb, GFP_ATOMIC);

	if (err == -ENODEV)
		netif_device_detach(netdev);
	else if (err)
		netdev_err(netdev, "failed resubmitting intr urb: %d\n", err);
}