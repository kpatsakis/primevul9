static void ems_usb_write_bulk_callback(struct urb *urb)
{
	struct ems_tx_urb_context *context = urb->context;
	struct ems_usb *dev;
	struct net_device *netdev;

	BUG_ON(!context);

	dev = context->dev;
	netdev = dev->netdev;

	/* free up our allocated buffer */
	usb_free_coherent(urb->dev, urb->transfer_buffer_length,
			  urb->transfer_buffer, urb->transfer_dma);

	atomic_dec(&dev->active_tx_urbs);

	if (!netif_device_present(netdev))
		return;

	if (urb->status)
		netdev_info(netdev, "Tx URB aborted (%d)\n", urb->status);

	netif_trans_update(netdev);

	/* transmission complete interrupt */
	netdev->stats.tx_packets++;
	netdev->stats.tx_bytes += can_get_echo_skb(netdev, context->echo_index,
						   NULL);

	/* Release context */
	context->echo_index = MAX_TX_URBS;

}