static void peak_usb_write_bulk_callback(struct urb *urb)
{
	struct peak_tx_urb_context *context = urb->context;
	struct peak_usb_device *dev;
	struct net_device *netdev;

	BUG_ON(!context);

	dev = context->dev;
	netdev = dev->netdev;

	atomic_dec(&dev->active_tx_urbs);

	if (!netif_device_present(netdev))
		return;

	/* check tx status */
	switch (urb->status) {
	case 0:
		/* transmission complete */
		netdev->stats.tx_packets++;
		netdev->stats.tx_bytes += context->data_len;

		/* prevent tx timeout */
		netif_trans_update(netdev);
		break;

	default:
		if (net_ratelimit())
			netdev_err(netdev, "Tx urb aborted (%d)\n",
				   urb->status);
	case -EPROTO:
	case -ENOENT:
	case -ECONNRESET:
	case -ESHUTDOWN:

		break;
	}

	/* should always release echo skb and corresponding context */
	can_get_echo_skb(netdev, context->echo_index);
	context->echo_index = PCAN_USB_MAX_TX_URBS;

	/* do wakeup tx queue in case of success only */
	if (!urb->status)
		netif_wake_queue(netdev);
}